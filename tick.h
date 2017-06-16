#pragma once

#include <thread>
#include <ev++.h>

class Tick
{
public:
	void start();
	void stop();
	uint32_t tick() {return _wTick;}

private:
	void timerThread();
	void timerCB(ev::timer &w, int revents);
	void workerThread();
	void workerCB(ev::async &w, int revents);

	std::thread *_tWorker;
	ev::async *_async;
	bool _stop;
	uint32_t _tick, _wTick;
};
