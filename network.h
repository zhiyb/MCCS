#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdint.h>
#include <string>
#include <unordered_set>
#include "handler.h"

class Network
{
public:
	Network();
	bool listen();
	void close();
	bool process();

	int err() {return _errno;}
	uint16_t nport() {return htons(25566);}
	uint32_t naddr() {return INADDR_ANY;}
	struct in6_addr naddr6() {return IN6ADDR_ANY_INIT;}

	std::string host() {return host(naddr(), nport());}
	static std::string host(uint32_t a, uint16_t p);
	static std::string saddrtostr(struct sockaddr_in *addr);

private:
	int listen4();
	int listen6();
	void gc();

	std::unordered_set<Handler *> handlers;
	int _errno;
	struct {
		int listen;
	} sd;
};
