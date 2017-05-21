#include <iostream>
#include <string>
#include "packet.h"
#include "client.h"
#include "packets/packets.h"
#include "protocols/p332_id.h"

using namespace Protocol::p332;

void Client::play(const Packet *p)
{
	switch (p->id()) {
	case ID::ClientSettings: {
		PktClientSettings pp(*p);
		if (pp.err())
			break;
		pp.dump();
		return;
	}
	case ID::PluginMsg: {
		PktPluginMsg pp(*p);
		if (pp.err())
			break;
		pp.dump();
		return;
	}
	case ID::KeepAlive:
		PktKeepAlive(*p).dump();
		return;
	}
	p->dump();
}
