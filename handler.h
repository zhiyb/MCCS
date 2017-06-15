#pragma once

#include <thread>
#include <deque>
#include <ev++.h>
#include "packet.h"
#include "client.h"

class Handler
{
public:
	Handler(int sd);
	bool closed() const {return _sd == -1;}

	std::thread *thread() const {return _th;}
	void thread(std::thread *th) {_th = th;}

	int err() const {return _errno;}

	static void threadFunc(Handler *h) {h->process();}
	void send(pkt_t *v);
	void disconnect();

private:
	void recv(pkt_t *v);
	int32_t readVarInt(pkt_t *pkt);

	void process();
	void sdCallbackR(ev::io &w, int revents);
	void sdCallbackW(ev::io &w, int revents);

	ev::io *sdWatcherR, *sdWatcherW;
	int32_t pktLength, pktSize;
	pkt_t pktRecv;
	std::deque<uint8_t> sendQueue;

	Client c;
	int _errno;
	int _sd;
	std::thread *_th;
};
