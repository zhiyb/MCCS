#ifndef PKT_PING_H
#define PKT_PING_H

#include <sstream>
#include "../packet.h"

class PktPing : public Packet
{
public:
	PktPing(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		_payload = readLong();
		if (err())
			goto error;
		return;

	error:
		_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << "Ping, payload " << _payload;
		Packet::dump(ss.str());
	}

	int32_t payload() {return _payload;}

private:
	int64_t _payload;
};

#endif
