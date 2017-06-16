#include "client.h"
#include "status.h"
#include "player.h"
#include "handler.h"
#include "packets/playpackets.h"
#include "protocols.h"
#include "protocols/id.h"
#include "logging.h"
#include "chat.h"

using namespace std;
using namespace Protocol;

void Client::play(const Packet *p)
{
	switch (p->id()) {
	case Play::Server::TeleportConfirm:
		if (PktTPConfirm(*p).id() == _tpID)
			player->teleportConfirm();
		return;
	case Play::Server::KeepAlive:
		if (PktKeepAlive(*p).id() == _keepAlive)
			hdr->feed();
		return;
	case Play::Server::ClientSettings: {
		PktClientSettings pp(*p);
		if (pp.err())
			break;
		player->locale(pp.locale());
		player->viewDistance(pp.viewDistance());
		player->chatMode(pp.mode() | (pp.colors() ? ChatMode::Colors : 0));
		player->skinDisplay(pp.skinParts());
		player->mainHand(pp.mainHand());
		player->spawn();
		return;
	}
	case Play::Server::PlayerPosition: {
		PktPlayerPosition pp(*p);
		if (pp.err())
			break;
		player->moveTo({pp.x(), pp.y(), pp.z()});
		return;
	}
	case Play::Server::PlayerPositionLook: {
		PktPlayerPositionLook pp(*p);
		if (pp.err())
			break;
		player->moveTo({pp.x(), pp.y(), pp.z()});
		player->lookAt({pp.yaw(), pp.pitch()});
		player->onGround(pp.onGround());
		return;
	}
	case Play::Server::PlayerLook: {
		PktPlayerLook pp(*p);
		if (pp.err())
			break;
		player->lookAt({pp.yaw(), pp.pitch()});
		player->onGround(pp.onGround());
		return;
	}
	case Play::Server::CloseWindow:
		PktCloseWindow(*p).dump();
		return;
	case Play::Server::PluginMessage: {
		PktPluginMsg pp(*p);
		if (pp.err())
			break;
		pp.dump();
		return;
	}
	case Play::Server::EntityAction:
		PktEntityAct(*p).dump();
		return;
	}
	p->dump();
}

void Client::playInit()
{
	player = new Player(this);
	if (!player->init(_playerName))
		hdr->disconnect();
}

void Client::join(eid_t eid, uint8_t gameMode, int32_t dimension, uint8_t difficulty, std::string level, bool debug) const
{
	// Join game
	pkt_t pkt;
	pktPushVarInt(&pkt, pktid(Play::Client::JoinGame));
	pktPushInt(&pkt, eid);
	pktPushUByte(&pkt, gameMode);
	pktPushInt(&pkt, dimension);
	pktPushUByte(&pkt, difficulty);
	pktPushUByte(&pkt, ::status.playersMax());
	pktPushString(&pkt, level);
	pktPushBoolean(&pkt, !debug);
	hdr->send(&pkt);
}

void Client::spawnPosition(const iPosition_t &position) const
{
	// Spawn position
	pkt_t pkt;
	pktPushVarInt(&pkt, pktid(Play::Client::SpawnPosition));
	pktPushPosition(&pkt, position);
	hdr->send(&pkt);
}

void Client::playerAbilities(uint8_t abilities, float speed, float fov) const
{
	// Player abilities
	pkt_t pkt;
	pktPushVarInt(&pkt, pktid(Play::Client::PlayerAbilities));
	pktPushByte(&pkt, abilities);	// Flags
	pktPushFloat(&pkt, speed);	// Speed
	pktPushFloat(&pkt, fov);	// FOV modifier
	hdr->send(&pkt);
}

void Client::playerPositionLook(const dPosition_t &pos, const fLook_t &look, uint8_t relative)
{
	_tpID = hdr->rand();
	// Player position and look
	pkt_t pkt;
	pktPushVarInt(&pkt, pktid(Play::Client::PlayerPositionLook));
	pktPushPosition(&pkt, pos);
	pktPushLook(&pkt, look);
	pktPushByte(&pkt, relative);	// Relative/absolute flags
	pktPushVarInt(&pkt, _tpID);	// Teleport ID
	hdr->send(&pkt);
}

void Client::disconnectPlayer(const Chat::Message &reason)
{
	_reason = reason.toText();
	pkt_t pkt;
	pktPushVarInt(&pkt, pktid(Play::Client::Disconnect));
	pktPushString(&pkt, reason.toJson());
	hdr->send(&pkt);
	hdr->disconnect();
}

void Client::sendNewChunk(const chunkCoord_t &c)
{
	// Send chunk data
	pkt_t pkt;
	pktPushVarInt(&pkt, pktid(Play::Client::ChunkData));
	pktPushInt(&pkt, c.x);
	pktPushInt(&pkt, c.z);
	pktPushBoolean(&pkt, true);		// Ground-up continuous
	pktPushVarInt(&pkt, 0x0001);		// Primary bit mask
	pushChunkSection(&pkt, c.x, c.z, true);	// Chunk section data
	pktPushVarInt(&pkt, 0);			// Block entities #
	pktPushByteArray(&pkt, 0, 0);		// Block entities NBT data
	hdr->send(&pkt);
}

void Client::unloadChunk(const chunkCoord_t &c)
{
	// Unload chunk
	pkt_t pkt;
	pktPushVarInt(&pkt, pktid(Play::Client::UnloadChunk));
	pktPushInt(&pkt, c.x);
	pktPushInt(&pkt, c.z);
	hdr->send(&pkt);

}

void Client::pushChunkSection(pkt_t *p, int32_t x, int32_t z, bool biome)
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
		unsigned long block = 2ul << 4;	// Grass
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
