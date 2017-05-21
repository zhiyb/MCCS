#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <vector>
#include <string>

typedef std::vector<uint8_t> pkt_t;

void pktPushBoolean(pkt_t *pkt, bool v);
void pktPushUByte(pkt_t *pkt, uint8_t v);
void pktPushInt(pkt_t *pkt, int32_t v);
void pktPushLong(pkt_t *pkt, int64_t v);
void pktPushVarInt(pkt_t *pkt, int32_t v);
void pktPushString(pkt_t *pkt, std::string str);
void pktPushByteArray(pkt_t *pkt, const void *p, const size_t size);

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
	int64_t readLong();
	int32_t readVarInt();
	int64_t readVarLong();
	std::string readString(uint32_t size);
	uint16_t readUShort();
	bool readByteArray(std::vector<uint8_t> *v, uint32_t size);

	int32_t _id;
	int _errno;
	bool _valid;

private:
	uint32_t len;
	const uint8_t *p;
};

#endif // PACKET_H
