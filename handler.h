#ifndef HANDLER_H
#define HANDLER_H

#include <thread>

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
	int _errno;
	int _sd;
	std::thread *_th;
};

#endif // HANDLER_H
