#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "packet.h"

void pktPushInt(pkt_t *pkt, int32_t v)
{
	int32_t tmp = ntohl(v);
	pkt->insert(pkt->end(), (uint8_t *)&tmp, (uint8_t *)&tmp + 4);
}

void pktPushLong(pkt_t *pkt, int64_t v)
{
	int32_t tmp = ntohl(v >> 32);
	pkt->insert(pkt->end(), (uint8_t *)&tmp, (uint8_t *)&tmp + 4);
	tmp = ntohl(v & 0xffffffff);
	pkt->insert(pkt->end(), (uint8_t *)&tmp, (uint8_t *)&tmp + 4);
}

void pktPushFloat(pkt_t *pkt, float v)
{
	int32_t i;
	memcpy(&i, &v, 4);
	pktPushInt(pkt, i);
}

void pktPushDouble(pkt_t *pkt, double v)
{
	int64_t i;
	memcpy(&i, &v, 8);
	pktPushLong(pkt, i);
}

void pktPushVarInt(pkt_t *pkt, int32_t v)
{
	uint32_t u = v;
	do {
		uint8_t c = u & 0x7f;
		u >>= 7;
		if (u)
			c |= 0x80;
		pkt->push_back(c);
	} while (u);
}

void pktPushString(pkt_t *pkt, std::string str)
{
	pktPushVarInt(pkt, str.size());
	pkt->insert(pkt->end(), str.begin(), str.end());
}

void pktPushPosition(pkt_t *pkt, int32_t x, int16_t y, int32_t z)
{
	int64_t v = (((int64_t)x & 0x3ffffff) << 38) |
		(((int64_t)y & 0xfff) << 26) | ((int64_t)z & 0x3ffffff);
	pktPushLong(pkt, v);
}

int8_t Packet::readByte()
{
	if (len < 1) {
		_errno = ENODATA;
		return 0;
	}
	int8_t c = *p++;
	len--;
	return c;
}

int32_t Packet::readInt()
{
	if (len < 4) {
		_errno = ENODATA;
		return 0;
	}
	int32_t v = 0;
	memcpy(&v, p, 4);
	p += 4;
	len -= 4;
	return v;
}

int64_t Packet::readLong()
{
	if (len < 8) {
		_errno = ENODATA;
		return 0;
	}
	int64_t v = 0;
	int32_t tmp;
	memcpy(&tmp, p, 4);
	v |= (int64_t)ntohl(tmp) << 32;
	p += 4;
	memcpy(&tmp, p, 4);
	v |= (int64_t)ntohl(tmp);
	p += 4;
	len -= 8;
	return v;
}

float Packet::readFloat()
{
	int32_t i = readInt();
	float v;
	memcpy(&v, &i, 4);
	return v;
}

double Packet::readDouble()
{
	int64_t i = readLong();
	double v;
	memcpy(&v, &i, 8);
	return v;
}

int32_t Packet::readVarInt()
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

int64_t Packet::readVarLong()
{
	int i, max = len > 10 ? 10 : len;
	int v = 0;
	for (i = 0; i != max;) {
		uint8_t c = *p++;
		v |= (uint64_t)(c & 0x7f) << (i++ * 7);
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

bool Packet::readByteArray(std::vector<uint8_t> *v, uint32_t size)
{
	if (len < size) {
		_errno = ENODATA;
		return false;
	}
	std::vector<uint8_t> vec(p, p + size);
	v->swap(vec);
	p += size;
	len -= size;
	return true;
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
	fprintf(stderr, "Packet%s (0x%02x) %s {%s}\n%s",
			valid() ? "" : " invalid",
			id(), msg.c_str(), ss.str().c_str(),
			err() ? strerror(err()) : "");
}
