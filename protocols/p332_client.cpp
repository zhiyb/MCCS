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
	case ID::Server::PlayerPosLook:
		PktPlayerPosLook(*p).dump();
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
	pktPushPosition(&pkt, 0, 0, 0);
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
	pktPushDouble(&pkt, 0.f);	// Y
	pktPushDouble(&pkt, 0.f);	// Z
	pktPushFloat(&pkt, 0.f);	// Yaw
	pktPushFloat(&pkt, 0.f);	// Pitch
	pktPushByte(&pkt, 0);		// Relative/absolute flags
	pktPushVarInt(&pkt, 0);		// Teleport ID
	hdr->sendPacket(&pkt);
}
