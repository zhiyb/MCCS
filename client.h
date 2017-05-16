#ifndef CLIENT_H
#define CLIENT_H

#include "packet.h"

class Handler;

class Client
{
public:
	Client();

	void packet(const pkt_t *v);
	void handshake(const Packet *p);

	void handler(Handler *h) {hdr = h;}

private:
	Handler *hdr;

	enum {Handshake = 0, Status, Login, Play} state;
	bool compressed;
};

#endif // CLIENT_H
