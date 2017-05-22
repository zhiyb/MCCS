#include <math.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../handler.h"
#include "../packets/packets.h"
#include "p332_client.h"
#include "p332_id.h"

using namespace Protocol::p332;

void ClientProtocol::play(const Packet *p)
{
	switch (p->id()) {
	case ID::Server::TPConfirm:
		PktTPConfirm(*p).dump();
		return;
	case ID::Server::ClientSettings: {
		PktClientSettings pp(*p);
		if (pp.err())
			break;
		pp.dump();
		return;
	}
	case ID::Server::PluginMsg: {
		PktPluginMsg pp(*p);
		if (pp.err())
			break;
		pp.dump();
		return;
	}
	case ID::Server::KeepAlive:
		PktKeepAlive(*p).dump();
		return;
	case ID::Server::PlayerPos:
		PktPlayerPos(*p).dump();
		return;
	case ID::Server::PlayerPosLook:
		PktPlayerPosLook(*p).dump();
		return;
	case ID::Server::PlayerLook:
		PktPlayerLook(*p).dump();
		return;
	}
	p->dump();
}

void ClientProtocol::playInit()
{
	// Join game
	pkt_t pkt;
	pktPushVarInt(&pkt, 0x23);		// ID = 0x23
	pktPushInt(&pkt, 123);			// Entity ID
	pktPushUByte(&pkt, 3);			// Gamemode spectator
	pktPushInt(&pkt, 0);			// Dimension overworld
	pktPushUByte(&pkt, 0);			// Difficulty peaceful
	pktPushUByte(&pkt, 0);			// Ignored
	pktPushString(&pkt, "default");		// Level type
	pktPushBoolean(&pkt, false);		// Reduced debug info
	hdr->sendPacket(&pkt);

	// Spawn position
	pkt.clear();
	pktPushVarInt(&pkt, ID::Client::SpawnPos);
	pktPushPosition(&pkt, 0, 20 * FP1, 0);
	hdr->sendPacket(&pkt);

	// Player abilities
	pkt.clear();
	pktPushVarInt(&pkt, ID::Client::PlayerAbility);
	pktPushByte(&pkt, 0x0f);	// Flags
	pktPushFloat(&pkt, 0.f);	// Speed
	pktPushFloat(&pkt, 0.f);	// FOV modifier
	hdr->sendPacket(&pkt);

	// Player position and look
	pkt.clear();
	pktPushVarInt(&pkt, ID::Client::PlayerPosLook);
	pktPushDouble(&pkt, 0.f);	// X
	pktPushDouble(&pkt, 18.f);	// Y
	pktPushDouble(&pkt, 0.f);	// Z
	pktPushFloat(&pkt, 0.f);	// Yaw
	pktPushFloat(&pkt, 0.f);	// Pitch
	pktPushByte(&pkt, 0);		// Relative/absolute flags
	pktPushVarInt(&pkt, 0);		// Teleport ID
	hdr->sendPacket(&pkt);

	sendNewChunks(0.f, 0.f);
}

void ClientProtocol::sendNewChunks(double x, double z)
{
	int32_t ix = round(x / 16.f) - 3, iz = round(z / 16.f) - 3;
	for (int i = 0; i != 7; i++)
		for (int j = 0; j != 7; j++)
			sendNewChunk(ix + i, iz + j);
}

void ClientProtocol::sendNewChunk(int32_t x, int32_t z)
{
	// Unload chunk
	pkt_t pkt;
	pktPushVarInt(&pkt, ID::Client::Unload);
	pktPushInt(&pkt, x);
	pktPushInt(&pkt, z);
	hdr->sendPacket(&pkt);

	// Send chunk data
	pkt.clear();
	pktPushVarInt(&pkt, ID::Client::Chunk);
	pktPushInt(&pkt, x);
	pktPushInt(&pkt, z);
	pktPushBoolean(&pkt, true);		// Ground-up continuous
	pktPushVarInt(&pkt, 0x0001);		// Primary bit mask
	pushChunkSection(&pkt, x, z, true);	// Chunk section data
	pktPushVarInt(&pkt, 0);			// Block entities #
	pktPushByteArray(&pkt, 0, 0);		// Block entities NBT data
	hdr->sendPacket(&pkt);
}

void ClientProtocol::pushChunkSection(pkt_t *p, int32_t x, int32_t z, bool biome)
{
	// Chunk data
	const auto bits = 13;
	pkt_t d;
	pktPushUByte(&d, bits);			// Bits per block
	pktPushVarInt(&d, 0);			// Palette length
	pktPushByteArray(&d, 0, 0);		// Palette data
	pktPushVarInt(&d, bits * 64);		// Data array length

	// Chunk section data array
	int cnt = 0;
	int64_t v = 0;
	for (int i = 0; i != 16 * 16 * 16; i++) {
		const auto block = 2ul << 4;	// Grass
		int off = (i * bits) % 64;
		v |= block << off;
		if (off + bits >= 64) {
			pktPushLong(&d, v);
			v = block >> (64 - off);
			cnt++;
		}
	}

	// Lighting
	uint8_t bl[2048], sl[2048];
	memset(bl, 0xff, 2048);			// Block light
	memset(sl, 0xff, 2048);			// Sky light
	pktPushByteArray(&d, bl, 2048);		// Block light
	pktPushByteArray(&d, sl, 2048);		// Sky light

	// Biome data
	uint8_t b[256];
	for (int i = 0; i != 256; i++)
		b[i] = 1;			// Plains biome
	pktPushByteArray(&d, b, 256);

	pktPushVarInt(p, d.size());
	pktPushByteArray(p, d.data(), d.size());
}
