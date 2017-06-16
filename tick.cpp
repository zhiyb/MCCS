#include "tick.h"
#include "logging.h"

using namespace std;

static const double period = 1;

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
	struct ev_loop *loop = ev_loop_new(EVFLAG_AUTO | EVFLAG_NOENV);
	ev::timer timer(loop);
	timer.set<Tick, &Tick::timerCB>(this);
	timer.set(period, period);
	timer.start();
	while (ev_run(loop, 0));
	ev_loop_destroy(loop);
}

void Tick::timerCB(ev::timer &w, int revents)
{
	if (_stop)
		w.stop();
	_tick++;
	_async->send();
}

void Tick::workerThread()
{
	struct ev_loop *loop = ev_loop_new(EVFLAG_AUTO | EVFLAG_NOENV);
	ev::async async(loop);
	async.set<Tick, &Tick::workerCB>(this);
	async.start();
	_async = &async;
	_wTick = _tick;
	thread tick(&Tick::timerThread, ref(*this));
	while (ev_run(loop, 0));
	tick.join();
	ev_loop_destroy(loop);
}

void Tick::workerCB(ev::async &w, int revents)
{
	uint32_t diff = _tick - _wTick;
	_wTick = _tick;
	if (_stop)
		w.stop();
	if (diff > 1)
		logger->warn("Skipping {} ticks", diff);
	logger->warn("Current tick: {}", tick());
}
