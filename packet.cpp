#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <arpa/inet.h>
#include <sstream>
#include <iomanip>
#include "packet.h"

int Packet::readVarInt()
{
	int i, max = len > 5 ? 5 : len;
	int v = 0;
	for (i = 0; i != max;) {
		uint8_t c = *p++;
		v |= (uint32_t)(c & 0x7f) << (i++ * 7);
		if (!(c & 0x80))
			break;
	}
	_errno = i == max && (*(p - 1) & 0x80) ? EOVERFLOW : 0;
	len -= i;
	return v;
}

std::string Packet::readString(uint32_t size)
{
	uint32_t len = readVarInt();
	if (err())
		return std::string();
	if (len > size * 4) {
		_errno = ENOBUFS;
		return std::string();
	}
	std::string s((const char *)p, len);
	this->len -= len;
	this->p += len;
	return s;
}

uint16_t Packet::readUShort()
{
	_errno = len < 2 ? ENODATA : 0;
	if (err())
		return 0;
	uint16_t s;
	memcpy(&s, p, 2);
	len -= 2;
	p += 2;
	return ntohs(s);
}

void Packet::dump(const std::string &msg) const
{
	std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(2);
	uint32_t len = this->len;
	const uint8_t *p = this->p;
	while (len--) {
		ss << "0x" << (unsigned int)*p++;
		if (len != 0)
			ss << ", ";
	}
	syslog(LOG_DEBUG, "Packet%s (0x%02x) %s {%s}\n%s",
			valid() ? "" : " invalid",
			id(), msg.c_str(), ss.str().c_str(),
			err() ? strerror(err()) : "");
}
