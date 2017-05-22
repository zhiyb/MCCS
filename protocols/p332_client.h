#ifndef PROTOCOL_332_CLIENT_H
#define PROTOCOL_332_CLIENT_H

#include "../client.h"

namespace Protocol {
namespace p332 {

class ClientProtocol : public Client
{
protected:
	virtual void play(const Packet *p);
	virtual void playInit();

private:
	void sendNewChunks(double x, double z);
	void sendNewChunk(int32_t x, int32_t z);
	void pushChunkSection(pkt_t *p, int32_t x, int32_t z, bool biome);
};

}
}

#endif
