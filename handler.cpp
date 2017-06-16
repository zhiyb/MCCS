#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "logging.h"
#include "network.h"
#include "handler.h"
#include "packet.h"
#include "protocols.h"

using std::vector;
using std::mutex;
using namespace Protocol;

Handler::Handler(int sd, int seed)
{
	_sd = sd;
	_aboutToDisconnect = false;
	ioSocketR = 0;
	ioSocketW = 0;
	tWatchdog = 0;
	tKeepAlive = 0;
	_rand.seed(seed);
}

void Handler::process()
{
#if 0
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	if (::getpeername(_sd, (struct sockaddr *)&addr, &len) == -1) {
		_errno = errno;
		logger->warn("Network error: {}", strerror(err()));
		close(_sd);
		_sd = -1;
		return;
	}
	logger->info("Connection established from {}", Network::saddrtostr(&addr).c_str());
#endif

	struct ev_loop *loop = ev_loop_new(EVFLAG_AUTO | EVFLAG_NOENV);
	ioSocketR = new ev::io(loop);
	ioSocketR->set<Handler, &Handler::ioSocketRCB>(this);
	ioSocketR->start(_sd, ev::READ);
	ioSocketW = new ev::io(loop);
	ioSocketW->set<Handler, &Handler::ioSocketWCB>(this);
	tWatchdog = new ev::timer(loop);
	tWatchdog->set<Handler, &Handler::tWatchdogCB>(this);
	tWatchdog->set(60, 30);
	tWatchdog->start();
	tKeepAlive = new ev::timer(loop);
	tKeepAlive->set<Handler, &Handler::tKeepAliveCB>(this);
	tKeepAlive->set(10, 10);
	tKeepAlive->start();
	c.handler(this);
	pktLength = 0;
	while (ev_run(loop, 0));
	// The destructor automatically stops the watcher if it is active
	delete ioSocketR;
	delete ioSocketW;
	delete tWatchdog;
	delete tKeepAlive;
	ev_loop_destroy(loop);
}

void Handler::disconnect(int error)
{
	error = error ?: err();
	error = (error == EAGAIN || error == EWOULDBLOCK) ? 0 : error;
	if (error == 0 && !_aboutToDisconnect && !sendQueue.empty()) {
		// Gracefully disconnect (wait for sendQueue empty)
		feed();
		_aboutToDisconnect = true;
		return;
	}

	c.logDisconnect(error);
	tWatchdog->stop();
	tKeepAlive->stop();
	ioSocketR->stop();
	ioSocketW->stop();
	close(_sd);
	_sd = -1;
}

void Handler::tWatchdogCB(ev::timer &w, int revents)
{
	disconnect(ETIMEDOUT);
}

void Handler::tKeepAliveCB(ev::timer &w, int revents)
{
	if (!_aboutToDisconnect)
		c.keepAlive();
}

void Handler::ioSocketRCB(ev::io &w, int revents)
{
	if (_aboutToDisconnect)
		return;
	recv(&pktRecv);
	if (err() == EAGAIN || err() == EWOULDBLOCK)
		return;
	else if (err()) {
		disconnect();
		return;
	}
	if (c.isEncrypted())
		c.decrypt(&pktRecv);
	if (c.isCompressed())
		;	// TODO: Compression support
	c.packet(&pktRecv);
	pktRecv.clear();
}

void Handler::ioSocketWCB(ev::io &w, int revents)
{
	lck.lock();
	if (sendQueue.empty())
		ioSocketW->stop();
	else if (!send() && err() != EAGAIN && err() != EWOULDBLOCK) {
		ioSocketW->stop();
		disconnect();
	}
	lck.unlock();
}

bool Handler::send()
{
	errno = 0;
	pkt_t data(sendQueue.begin(), sendQueue.end());
	ssize_t s = ::send(_sd, data.data(), data.size(), MSG_NOSIGNAL);
	if (s == -1)
		_errno = errno;
	else if (s == 0)
		_errno = errno ?: ECONNABORTED;
	else {
		sendQueue.erase(sendQueue.begin(), sendQueue.begin() + s);
		if (sendQueue.empty()) {
			ioSocketW->stop();
			_errno = 0;
			return true;
		}
		_errno = EAGAIN;
	}
	return false;
}

void Handler::send(pkt_t *v)
{
	if (_aboutToDisconnect)
		return;
	pktid_t id = Packet(v).id();
	if (id < 0) {
		logger->warn("Ignored packet of type {} size {}", protocols.hashToStr(-id), v->size());
		return;
	}
	pkt_t pkt;
	pktPushVarInt(&pkt, v->size());
	if (c.isEncrypted()) {
		c.encrypt(&pkt);
		c.encryptAppend(v, &pkt);
	} else
		pktPushByteArray(&pkt, v->data(), v->size());
	lck.lock();
	sendQueue.insert(sendQueue.end(), pkt.begin(), pkt.end());
	ioSocketW->start(_sd, ev::WRITE);
	lck.unlock();
}

void Handler::recv(pkt_t *v)
{
	if (pktLength == 0) {
		int32_t len = readVarInt(v);
		if (err())
			return;
		pktLength = len;
		pktSize = 0;
		v->resize(len);
		if (len == 0)
			return;
	}
	errno = 0;
	ssize_t s = ::recv(_sd, v->data() + pktSize, pktLength, 0);
	if (s == -1)
		_errno = errno;
	else if (s == 0)
		_errno = errno ?: ECONNABORTED;
	else {
		pktLength -= s;
		pktSize += s;
		_errno = pktLength != 0 ? EAGAIN : 0;
	}
}

int32_t Handler::readVarInt(pkt_t *pkt)
{
	size_t size = pkt->size();
	const uint8_t *ptr = pkt->data();
	_errno = 0;
	uint32_t v = 0;
	int i;
	for (i = 0; i != 5; i++) {
		uint8_t c;
		if (size) {
			c = *ptr++;
			size--;
		} else {
			errno = 0;
			ssize_t s = ::recv(_sd, &c, 1, 0);
			if (s != 1) {
				_errno = errno ?: ECONNABORTED;
				return 0;
			}
			if (this->c.isEncrypted())
				c = this->c.decrypt(c);
			pkt->push_back(c);
		}
		v |= (uint32_t)(c & 0x7f) << (i * 7);
		if (!(c & 0x80))
			break;
		else if (i == 4) {
			_errno = ERANGE;
			return 0;
		}
	}
	return v;
}
