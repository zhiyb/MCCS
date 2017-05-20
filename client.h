#ifndef CLIENT_H
#define CLIENT_H

#include "packet.h"

class Handler;

class Client
{
public:
	Client();

	void packet(const pkt_t *v);
	void handler(Handler *h) {hdr = h;}

private:
	void handshake(const Packet *p);
	void status(const Packet *p);

	Handler *hdr;

	enum {Handshake = 0, Status, Login, Play} state;
	bool compressed;
};

#endif // CLIENT_H
