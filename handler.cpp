#include <unistd.h>
#include <errno.h>
#include <string.h>
#ifdef __WIN32__
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "logging.h"
#include "network.h"
#include "handler.h"
#include "packet.h"
#include "protocols.h"

using namespace std;
using namespace uvw;
using namespace Protocol;

Handler::Handler(int sd, int seed)
{
	_sd = sd;
	_aboutToDisconnect = false;
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

	auto loop = Loop::create();

	ioSocket = loop->resource<TcpHandle>();
	ioSocket->on<DataEvent>([&](const DataEvent &e, TcpHandle &) {
		if (_aboutToDisconnect)
			return;
		size_t len = 0;
recv:
		recv(&pktRecv, e, &len);
		if (err()) {
			if (err() != EAGAIN)
				disconnect();
			return;
		}
		if (c.isEncrypted())
			c.decrypt(&pktRecv);
		if (c.isCompressed())
			;	// TODO: Compression support
		c.packet(&pktRecv);
		pktRecv.clear();
		if (len != e.length)
			goto recv;
	});
	ioSocket->on<EndEvent>([&](const EndEvent &, TcpHandle &) {
		disconnect(ECONNABORTED);
	});
	ioSocket->on<ShutdownEvent>([&](const ShutdownEvent &, TcpHandle &) {
		// Force disconnect
		_aboutToDisconnect = true;
		disconnect();
	});
	ioSocket->on<ErrorEvent>([&](const ErrorEvent &e, TcpHandle &) {
		disconnect(e.code());
	});
	ioSocket->open(_sd);
	ioSocket->read();

	ioEvent = loop->resource<AsyncHandle>();
	ioEvent->on<AsyncEvent>([&](const AsyncEvent &, AsyncHandle &) {
		if (_aboutToDisconnect || pktQueue.empty())
			return;
		lck.lock();
		while (!pktQueue.empty()) {
			auto &pkt = pktQueue.front();
			ioSocket->write(move(pkt.first), pkt.second);
			pktQueue.pop();
		}
		lck.unlock();
	});

	tWatchdog = loop->resource<TimerHandle>();
	tWatchdog->on<TimerEvent>([&](const TimerEvent &e, TimerHandle &) {
		disconnect(ETIMEDOUT);
	});
	tWatchdog->start(chrono::seconds(60), chrono::seconds(30));

	tKeepAlive = loop->resource<TimerHandle>();
	tKeepAlive->on<TimerEvent>([&](const TimerEvent &e, TimerHandle &) {
		if (!_aboutToDisconnect)
			c.keepAlive();
	});
	tKeepAlive->start(chrono::seconds(10), chrono::seconds(10));

	c.handler(this);
	pktLength = 0;
	loop->run();
}

void Handler::disconnect(int error)
{
	error = error ?: err();
	error = (error == EAGAIN || error == EWOULDBLOCK) ? 0 : error;
	if (error == 0 && !_aboutToDisconnect) {
		// Gracefully disconnect
		feed();
		ioSocket->shutdown();
		_aboutToDisconnect = true;
		return;
	}

	c.logDisconnect(error);
	ioEvent->close();
	ioSocket->close();
	tWatchdog->close();
	tKeepAlive->close();
	close(_sd);
	_sd = -1;
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
	int size = pkt.size() + v->size();
	auto data = new char[size];
	if (c.isEncrypted()) {
		c.encrypt(data, pkt.data(), pkt.size());
		c.encrypt(data + pkt.size(), v->data(), v->size());
	} else {
		memcpy(data, pkt.data(), pkt.size());
		memcpy(data + pkt.size(), v->data(), v->size());
	}
	lck.lock();
	pktQueue.push(make_pair(unique_ptr<char[]>(data), size));
	lck.unlock();
	ioEvent->send();
}

void Handler::recv(pkt_t *v, const DataEvent &e, size_t *len)
{
	if (pktLength == 0) {
		pktLength = readVarInt(v, e, len);
		if (err())
			return;
		pktSize = 0;
		v->resize(pktLength);
	}
	size_t l = min((size_t)pktLength, e.length - *len);
	memcpy(v->data() + pktSize, e.data.get() + *len, l);
	pktLength -= l;
	pktSize += l;
	*len += l;
	_errno = pktLength != 0 ? EAGAIN : 0;
}

int32_t Handler::readVarInt(pkt_t *pkt, const DataEvent &e, size_t *len)
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
			if (e.length == *len) {
				_errno = EAGAIN;
				return 0;
			}
			c = *(e.data.get() + (*len)++);
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
