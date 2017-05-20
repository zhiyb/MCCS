#ifndef HANDLER_H
#define HANDLER_H

#include <thread>
#include "packet.h"
#include "client.h"

class Handler
{
public:
	Handler(int sd);
	void process();
	bool closed() const {return _sd == -1;}

	std::thread *thread() const {return _th;}
	void thread(std::thread *th) {_th = th;}

	int err() const {return _errno;}

	static void threadFunc(Handler *h) {h->process();}
	void sendPacket(const pkt_t *v);

private:
	void readPacket(pkt_t *v);
	uint32_t readVarInt();
	uint32_t readVarLong();

	Client c;
	int _errno;
	int _sd;
	std::thread *_th;
};

#endif // HANDLER_H
