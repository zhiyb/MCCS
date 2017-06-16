#pragma once

#include <thread>
#include <deque>
#include <mutex>
#include <random>
#include <ev++.h>
#include "packet.h"
#include "client.h"

class Handler
{
public:
	Handler(int sd, int seed = 0);
	bool closed() const {return _sd == -1;}

	std::thread *thread() const {return _th;}
	void thread(std::thread *th) {_th = th;}
	void feed() {if (!_aboutToDisconnect) tWatchdog->again();}

	uint32_t rand() {return _rand();}
	int err() const {return _errno;}

	static void threadFunc(Handler *h) {h->process();}
	void send(pkt_t *v);
	void disconnect(int error = 0);

private:
	bool send();
	void recv(pkt_t *v);
	int32_t readVarInt(pkt_t *pkt);

	void process();
	void ioSocketRCB(ev::io &w, int revents);
	void ioSocketWCB(ev::io &w, int revents);
	void tWatchdogCB(ev::timer &w, int revents);
	void tKeepAliveCB(ev::timer &w, int revents);

	std::thread *_th;
	std::minstd_rand _rand;

	ev::io *ioSocketR, *ioSocketW;
	ev::timer *tWatchdog, *tKeepAlive;

	int32_t pktLength, pktSize;
	pkt_t pktRecv;
	std::deque<uint8_t> sendQueue;
	std::mutex lck;

	Client c;
	int _errno;
	int _sd;
	bool _aboutToDisconnect;
};
