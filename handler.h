#pragma once

#include <thread>
#include <deque>
#include <random>
#include <ev++.h>
#include "packet.h"
#include "client.h"

class Handler
{
public:
	Handler(int sd, int seed = 0);
	~Handler();
	bool closed() const {return _sd == -1;}

	std::thread *thread() const {return _th;}
	void thread(std::thread *th) {_th = th;}
	struct ev_loop *evLoop() {return loop;}
	void feed() {tWatchdog->again();}

	uint32_t rand() {return _rand();}
	int err() const {return _errno;}

	static void threadFunc(Handler *h) {h->process();}
	void send(pkt_t *v);
	void disconnect(int error = 0);

private:
	void recv(pkt_t *v);
	int32_t readVarInt(pkt_t *pkt);

	void process();
	void ioSocketRCB(ev::io &w, int revents);
	void ioSocketWCB(ev::io &w, int revents);
	void tWatchdogCB(ev::timer &w, int revents);
	void tKeepAliveCB(ev::timer &w, int revents);

	std::thread *_th;
	std::minstd_rand _rand;

	struct ev_loop *loop;
	ev::io *ioSocketR, *ioSocketW;
	ev::timer *tWatchdog, *tKeepAlive;

	int32_t pktLength, pktSize;
	pkt_t pktRecv;
	std::deque<uint8_t> sendQueue;

	Client c;
	int _errno;
	int _sd;
};
