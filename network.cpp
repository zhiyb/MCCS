#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sstream>
#include "network.h"
#include "handler.h"
#include "logging.h"

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
	// Listening on IPv6 will also bind to IPv4 (see flag IPV6_V6ONLY)
	sd.listen = listen6();
	return sd.listen != -1;
}

int Network::listen4()
{
	int flag = 1;
	int sd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		logger->warn("Cannot create IPv4 socket: {}", strerror(errno));
		goto close;
	}
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
		logger->warn("Cannot configure IPv4 socket: {}", strerror(errno));
		goto close;
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = nport();
	addr.sin_addr.s_addr = naddr();
	if (::bind(sd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		logger->warn("Cannot bind IPv4 socket: {}", strerror(errno));
		goto close;
	}
	if (::listen(sd, 16) == -1) {
		logger->warn("Cannot listen on IPv4: {}", strerror(errno));
		goto close;
	}
	return sd;

close:
	if (sd != -1)
		::close(sd);
	_errno = errno;
	return -1;
}

int Network::listen6()
{
	int flag = 1;
	int sd = ::socket(AF_INET6, SOCK_STREAM, 0);
	if (sd == -1) {
		logger->warn("Cannot create IPv6 socket: {}", strerror(errno));
		goto close;
	}
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
		logger->warn("Cannot configure IPv6 socket: {}", strerror(errno));
		goto close;
	}
	struct sockaddr_in6 addr;
	memset(&addr, 0, sizeof(struct sockaddr_in6));
	addr.sin6_family = AF_INET6;
	addr.sin6_port = nport();
	addr.sin6_addr = naddr6();
	if (::bind(sd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		logger->warn("Cannot bind IPv6 socket: {}", strerror(errno));
		goto close;
	}
	if (::listen(sd, 16) == -1) {
		logger->warn("Cannot listen on IPv6: {}", strerror(errno));
		goto close;
	}
	return sd;

close:
	if (sd != -1)
		::close(sd);
	_errno = errno;
	return -1;
}

bool Network::process()
{
	logger->info("Network process started");
	while (true) {
		int s = accept4(sd.listen, NULL, 0, SOCK_NONBLOCK);
		if (s < 0)
			goto error;
		int flag = 1;
		setsockopt(s, IPPROTO_TCP, TCP_NODELAY,
				(void *)&flag, sizeof(flag));
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
