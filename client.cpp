#include <string.h>
#include <syslog.h>
#include "client.h"
#include "handler.h"
#include "status.h"
#include "packets/packets.h"

Client::Client()
{
	compressed = false;
	state = Handshake;
}

void Client::disconnect(int e)
{
	if (!_playerName.empty())
		syslog(LOG_INFO, "Player %s disconnected: %s\n",
				_playerName.c_str(), strerror(e));
}

void Client::packet(const pkt_t *v)
{
	if (compressed)
		;

	Packet *p = new Packet(v);
	if (p->err())
		goto drop;

	switch (state) {
	case Handshake:
		handshake(p);
		break;
	case Status:
		status(p);
		break;
	case Login:
		login(p);
		break;
	default:
		goto drop;
	}
	delete p;
	return;

drop:
	p->dump();
	delete p;
}

void Client::handshake(const Packet *p)
{
	switch (p->id()) {
	case 0x00: {	// Handshake
		PktHandshake phs(*p);
		if (phs.err())
			goto drop;
		switch (phs.next()) {
		case 1:
			state = Status;
			break;
		case 2:
			state = Login;
			break;
		default:
			goto drop;
		}
		break;
	}
	default:
		goto drop;
	}
	return;

drop:
	p->dump();
}

void Client::status(const Packet *p)
{
	pkt_t pkt;
	switch (p->id()) {
	case 0x00:	// Status request
		pktPushVarInt(&pkt, 0x00);		// ID = 0x00
		pktPushString(&pkt, ::status.toJson());	// JSON response
		break;
	case 0x01: {	// Ping
		PktPing pp(*p);
		if (pp.err())
			goto drop;
		pktPushVarInt(&pkt, 0x01);		// ID = 0x01
		pktPushLong(&pkt, pp.payload());	// Ping Pong!
		break;
	}
	default:
		goto drop;
	}
	hdr->sendPacket(&pkt);
	return;

drop:
	p->dump();
}

void Client::login(const Packet *p)
{
	pkt_t pkt;
	switch (p->id()) {
	case 0x00: {	// Login start
		PktLoginStart pls(*p);
		if (pls.err())
			goto drop;
		_playerName = pls.playerName();
		pls.dump();
		syslog(LOG_INFO, "Player %s logging in...\n",
				_playerName.c_str());

		// Disconnect
		pktPushVarInt(&pkt, 0x00);		// ID = 0x00
		pktPushString(&pkt, "{\"text\":\"Not yet implemented\"}");

		// Encryption request
		break;
	}
	default:
		   goto drop;
	}
	hdr->sendPacket(&pkt);
	return;

drop:
	p->dump();
}
