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
using namespace Protocol;

Handler::Handler(int sd)
{
	_sd = sd;
}

void Handler::process()
{
	struct ev_loop *loop = EV_DEFAULT;
	sdWatcherR = new ev::io(loop);
	sdWatcherR->set<Handler, &Handler::sdCallbackR>(this);
	sdWatcherR->start(_sd, ev::READ);
	sdWatcherW = new ev::io(loop);
	sdWatcherW->set<Handler, &Handler::sdCallbackW>(this);

	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	if (::getpeername(_sd, (struct sockaddr *)&addr, &len) == -1) {
		_errno = errno;
		logger->warn("Network error: {}", strerror(err()));
		disconnect();
		delete sdWatcherR;
		return;
	}

	//logger->info("Connection established from {}", Network::saddrtostr(&addr).c_str());
	c.handler(this);
	pktLength = 0;
	ev_run(loop, 0);
	delete sdWatcherR;
}

void Handler::sdCallbackR(ev::io &w, int revents)
{
	if (!(revents & EV_READ))
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

void Handler::sdCallbackW(ev::io &w, int revents)
{
	if (!(revents & EV_WRITE))
		return;
	if (sendQueue.empty()) {
		sdWatcherW->stop();
		return;
	}
	errno = 0;
	pkt_t data(sendQueue.begin(), sendQueue.end());
	ssize_t s = ::send(_sd, data.data(), data.size(), MSG_NOSIGNAL);
	if (s == -1)
		_errno = errno;
	else if (s == 0)
		_errno = errno ?: ECONNABORTED;
	else {
		sendQueue.erase(sendQueue.begin(), sendQueue.begin() + s);
		_errno = sendQueue.empty() ? 0 : EAGAIN;
		if (sendQueue.empty())
			sdWatcherW->stop();
	}
	if (err() && err() != EAGAIN && err() != EWOULDBLOCK)
		disconnect();
}

void Handler::send(pkt_t *v)
{
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
	sendQueue.insert(sendQueue.end(), pkt.begin(), pkt.end());
	sdWatcherW->start(_sd, ev::WRITE);
}

void Handler::disconnect()
{
	c.logDisconnect(err());
	sdWatcherR->stop();
	sdWatcherW->stop();
	close(_sd);
	_sd = -1;
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
