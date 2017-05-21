#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "network.h"
#include "handler.h"
#include "packet.h"
#include "client.h"

using std::vector;

Handler::Handler(int sd)
{
	_sd = sd;
}

void Handler::process()
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	if (::getpeername(_sd, (struct sockaddr *)&addr, &len) == -1)
		goto error;
	//syslog(LOG_INFO, "[%p] Connection established from %s\n", this,
	//		Network::saddrtostr(&addr).c_str());
	c.handler(this);

	for (;;) {
		pkt_t v;
		readPacket(&v);
		if (c.isCompressed())
			;	// TODO: Compression support
		if (err()) {
			c.disconnect(err());
			goto close;
		}
		if (c.isEncrypted())
			c.decrypt(&v);
		c.packet(&v);
	}
	_errno = 0;
	goto close;

error:
	_errno = errno;
	syslog(LOG_WARNING, "[%p] Error: %s\n", this, strerror(err()));
close:
	close(_sd);
	_sd = -1;
}

void Handler::sendPacket(pkt_t *v)
{
	pkt_t header;
	pktPushVarInt(&header, v->size());
	pktPushByteArray(&header, v->data(), v->size());
	if (c.isEncrypted())
		c.encrypt(&header);
	write(_sd, header.data(), header.size());
	//write(_sd, v->data(), v->size());
}

void Handler::disconnect()
{
	close(_sd);
	_sd = -1;
}

void Handler::readPacket(pkt_t *v)
{
	uint32_t len = readVarInt();
	if (err())
		return;
	v->resize(len);
	ssize_t s = read(_sd, v->data(), len);
	if (s != len)
		_errno = s == -1 ? errno : ECONNABORTED;
}

uint32_t Handler::readVarInt()
{
	_errno = 0;
	uint32_t v = 0;
	int i;
	for (i = 0; i != 5; i++) {
		uint8_t c;
		ssize_t s = read(_sd, &c, 1);
		if (s != 1) {
			_errno = s == 0 ? ECONNABORTED : errno;
			return 0;
		}
		if (this->c.isEncrypted())
			c = this->c.decrypt(c);
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
