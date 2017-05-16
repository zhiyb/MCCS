#ifndef PKT_HANDSHAKE_H
#define PKT_HANDSHAKE_H

#include <sstream>
#include "../packet.h"

class PktHandshake : public Packet
{
public:
	PktHandshake(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		_proto = readVarInt();
		if (err())
			goto error;
		_addr = readString(255);
		if (err())
			goto error;
		_port = readUShort();
		if (err())
			goto error;
		_next = readVarInt();
		if (err())
			goto error;
		return;

	error:
		_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << "Handshake, proto " << _proto << ", "
			<< _addr << ':' << _port << ", next " << _next;
		Packet::dump(ss.str());
	}

	int32_t next() {return _next;}

private:
	int32_t _proto;
	std::string _addr;
	uint16_t _port;
	int32_t _next;
};

#endif
