#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <vector>
#include <string>

typedef std::vector<uint8_t> pkt_t;
typedef std::vector<uint8_t> byteArray_t;

static inline void pktPushBoolean(pkt_t *pkt, bool v) {pkt->push_back(v ? 0x01 : 0x00);}
static inline void pktPushByte(pkt_t *pkt, int8_t v) {pkt->push_back(v);}
static inline void pktPushUByte(pkt_t *pkt, uint8_t v) {pktPushByte(pkt, v);}
void pktPushInt(pkt_t *pkt, int32_t v);
void pktPushLong(pkt_t *pkt, int64_t v);
void pktPushFloat(pkt_t *pkt, float v);
void pktPushDouble(pkt_t *pkt, double v);
void pktPushVarInt(pkt_t *pkt, int32_t v);
void pktPushString(pkt_t *pkt, std::string str);
static inline void pktPushByteArray(pkt_t *pkt, const void *p, const size_t s) {pkt->insert(pkt->end(), (char *)p, (char *)p + s);}
void pktPushPosition(pkt_t *pkt, int32_t x, int16_t y, int32_t z);

class Packet
{
public:
	Packet(const pkt_t *pkt) : _errno(0)
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
	bool readBoolean() {return readUByte() == 0x00 ? false : true;}
	int8_t readByte();
	uint8_t readUByte() {return (uint8_t)readByte();}
	int32_t readInt();
	int64_t readLong();
	float readFloat();
	double readDouble();
	int32_t readVarInt();
	int64_t readVarLong();
	std::string readString(uint32_t size);
	uint16_t readUShort();
	bool readByteArray(byteArray_t *v, uint32_t size);

	uint32_t length() const {return len;}

	int32_t _id;
	int _errno;
	bool _valid;

private:
	uint32_t len;
	const uint8_t *p;
};

#endif // PACKET_H
