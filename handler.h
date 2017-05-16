#ifndef HANDLER_H
#define HANDLER_H

#include <stdint.h>
#include <vector>
#include <thread>

typedef std::vector<uint8_t> pkt_t;

class Handler
{
public:
	Handler(int sd);
	void process();
	bool closed() {return _sd == -1;}

	std::thread *thread() {return _th;}
	void thread(std::thread *th) {_th = th;}

	int err() {return _errno;}

	static void threadFunc(Handler *h) {h->process();}

private:
	void dumpPacket(pkt_t *v);

	void readPacket(pkt_t *v);
	uint32_t readVarInt();
	uint32_t readVarLong();

	int _errno;
	int _sd;
	std::thread *_th;
};

#endif // HANDLER_H
