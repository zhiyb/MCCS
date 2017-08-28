#pragma once

#include <thread>
#include <uvw.hpp>

class Tick
{
public:
	void start();
	void stop();
	uint32_t tick() {return _wTick;}

private:
	void timerThread();
	void workerThread();

	std::thread *_tWorker;
	std::shared_ptr<uvw::AsyncHandle> _async;
	bool _stop;
	uint32_t _tick, _wTick;
};
