#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <random>
#include <uvw.hpp>
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
	void process();
	void recv(pkt_t *v, const uvw::DataEvent &e, size_t *len);
	int32_t readVarInt(pkt_t *pkt, const uvw::DataEvent &e, size_t *len);

	std::thread *_th;
	std::minstd_rand _rand;

	std::shared_ptr<uvw::TcpHandle> ioSocket;
	std::shared_ptr<uvw::AsyncHandle> ioEvent;
	std::shared_ptr<uvw::TimerHandle> tWatchdog, tKeepAlive;

	int32_t pktLength, pktSize;
	pkt_t pktRecv;
	std::queue<std::pair<std::unique_ptr<char[]>, int> > pktQueue;
	std::mutex lck;

	Client c;
	int _errno;
	int _sd;
	bool _aboutToDisconnect;
};
