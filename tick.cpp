#include <chrono>
#include "tick.h"
#include "server.h"

using namespace std;
using namespace uvw;

static const auto period = chrono::milliseconds(50);

void Tick::start()
{
	_stop = false;
	_tick = 0;
	_tWorker = new thread(&Tick::workerThread, ref(*this));
}

void Tick::stop()
{
	_stop = true;
	_tWorker->join();
	delete _tWorker;
}

void Tick::timerThread()
{
	auto loop = Loop::create();
	shared_ptr<TimerHandle> timer = loop->resource<TimerHandle>();
	timer->on<TimerEvent>([&](const TimerEvent &e, TimerHandle &timer) {
		if (_stop)
			timer.close();
		_tick++;
		_async->send();
	});
	timer->start(period, period);
	loop->run();
}

void Tick::workerThread()
{
	auto loop = Loop::create();
	_async = loop->resource<AsyncHandle>();
	_async->on<AsyncEvent>([&](const AsyncEvent &e, AsyncHandle &async) {
		if (_stop)
			async.close();
		uint32_t diff = _tick - _wTick;
		_wTick = _tick;
		server->tick(diff);
	});
	_wTick = _tick;
	thread tick(&Tick::timerThread, ref(*this));
	loop->run();
	tick.join();
}
