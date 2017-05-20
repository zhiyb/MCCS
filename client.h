#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "packet.h"

class Handler;

class Client
{
public:
	Client();
	void disconnect(int e);
	void packet(const pkt_t *v);
	void handler(Handler *h) {hdr = h;}

private:
	void handshake(const Packet *p);
	void status(const Packet *p);
	void login(const Packet *p);

	Handler *hdr;

	enum {Handshake = 0, Status, Login, Play} state;
	bool compressed;
	std::string _playerName;
};

#endif // CLIENT_H
