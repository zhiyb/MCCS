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
	PktHandshake *phs = 0;
	switch (p->id()) {
	case 0x00:	// Handshake
		p = phs = new PktHandshake(*p);
		if (phs->err())
			goto drop;
		switch (phs->next()) {
		case 1:
			state = Status;
			break;
		case 2:
			state = Login;
			break;
		default:
			goto drop;
		}
		p->dump();
		delete phs;
		break;
	default:
		goto drop;
	}
	return;

drop:
	p->dump();
	if (phs)
		delete phs;
}

void Client::status(const Packet *p)
{
	pkt_t pkt;
	switch (p->id()) {
	case 0x00:	// Status request
		pktPushVarInt(&pkt, 0x00);		// ID = 0x00
		pktPushString(&pkt, ::status.toJson());	// JSON response
		hdr->sendPacket(&pkt);
		break;
	default:
		goto drop;
	}
	return;

drop:
	p->dump();
}
