#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <sstream>
#include "network.h"
#include "handler.h"

Network::Network()
{
	sd.listen = -1;
}

void Network::close()
{
	if (sd.listen != -1)
		::close(sd.listen);
	for (auto h: handlers) {
		h->thread()->join();
		delete h->thread();
		delete h;
	}
}

bool Network::listen()
{
	int opt = 1;
	sd.listen = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sd.listen == -1)
		goto error;
	setsockopt(sd.listen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = nport();
	addr.sin_addr.s_addr = naddr();
	if (::bind(sd.listen, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		goto close;
	if (::listen(sd.listen, 16) == -1)
		goto close;
	return true;

close:
	::close(sd.listen);
error:
	_errno = errno;
	return false;
}

bool Network::process()
{
	syslog(LOG_INFO, "Network process started\n");
	while (true) {
		int s = ::accept(sd.listen, NULL, NULL);
		if (s < 0)
			goto error;
		Handler *h = new Handler(s);
		std::thread *th = new std::thread(Handler::threadFunc, h);
		h->thread(th);
		handlers.insert(h);
		gc();
	}
	return true;
error:
	_errno = errno;
	return false;
}

void Network::gc()
{
	for (auto it = handlers.begin(); it != handlers.end();) {
		auto tmpit = it++;
		if ((*tmpit)->closed()) {
			(*tmpit)->thread()->join();
			delete (*tmpit)->thread();
			delete (*tmpit);
			handlers.erase(tmpit);
		}
	}
}

std::string Network::host(uint32_t addr, uint16_t port)
{
	std::stringstream ss;
	ss << (addr & 0xff) << '.' << ((addr >> 8) & 0xff) << '.'
		<< ((addr >> 16) & 0xff) << '.' << ((addr >> 24) & 0xff)
		<< ':' << ntohs(port);
	return ss.str();
}

std::string Network::saddrtostr(struct sockaddr_in *addr)
{
	return host(addr->sin_addr.s_addr, addr->sin_port);
}
