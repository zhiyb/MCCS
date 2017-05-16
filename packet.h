#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <vector>
#include <string>

typedef std::vector<uint8_t> pkt_t;

class Packet
{
public:
	Packet(const pkt_t *pkt) : _valid(false)
	{
		len = pkt->size();
		p = pkt->data();
		_id = readVarInt();
		_valid = !err();
	}
	virtual ~Packet() {}

	int err() const {return _errno;}
	virtual bool valid() const {return _valid;}
	int32_t id() const {return _id;}

	virtual void dump() const {dump("dump");}
	void dump(const std::string &s) const;

protected:
	int readVarInt();
	std::string readString(uint32_t size);
	uint16_t readUShort();

	int32_t _id;
	int _errno;
	bool _valid;

private:
	uint32_t len;
	const uint8_t *p;
};

#endif // PACKET_H
