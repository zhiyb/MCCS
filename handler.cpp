#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sstream>
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
	syslog(LOG_INFO, "[%p] Connection established from %s\n", this,
			Network::saddrtostr(&addr).c_str());
	c.handler(this);

	for (;;) {
		pkt_t v;
		readPacket(&v);
		if (err())
			goto close;
		c.packet(&v);
	}
	_errno = 0;
	goto close;

error:
	_errno = errno;
close:
	if (err())
		syslog(LOG_WARNING, "[%p] Error: %s\n", this, strerror(err()));
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
	uint8_t buf[5], *p = buf;
	int i;
	for (i = 0; i != 5; i++) {
		uint8_t c;
		ssize_t s = read(_sd, &c, 1);
		if (s != 1) {
			_errno = s == -1 ? errno : ECONNABORTED;
			return 0;
		}
		*p++ = c;
		if (!(c & 0x80))
			break;
	}
	if (buf[4] & 0x80) {
		_errno = ERANGE;
		return 0;
	}

	uint32_t v = 0;
	p = buf;
	for (i = 0; i != 5 * 7; i += 7) {
		uint8_t c = *p++;
		v |= (uint32_t)(c) << i;
		if (!(c & 0x80))
			break;
	}
	return v;
}

uint32_t Handler::readVarLong()
{
	_errno = 0;
	uint8_t buf[10], *p = buf;
	int i;
	for (i = 0; i != 10; i++) {
		uint8_t c;
		ssize_t s = read(_sd, &c, 1);
		if (s != 1) {
			_errno = s == -1 ? errno : ECONNABORTED;
			return 0;
		}
		*p++ = c;
		if (!(c & 0x80))
			break;
	}
	if (buf[9] & 0x80) {
		_errno = ERANGE;
		return 0;
	}

	uint64_t v = 0;
	p = buf;
	for (i = 0; i != 10 * 7; i += 7) {
		uint8_t c = *p;
		v |= (uint32_t)(c) << i;
		if (!(c & 0x80))
			break;
	}
	return v;
}
