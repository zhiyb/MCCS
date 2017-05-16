#include <syslog.h>
#include <unistd.h>
#include "network.h"
#include "handler.h"

Handler::Handler(int sd)
{
	_sd = sd;
}

void Handler::process()
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	if (::getpeername(_sd, (struct sockaddr *)&addr, &len) == -1)
		goto error;
	syslog(LOG_INFO, "Connection established from %s\n",
			Network::saddrtostr(&addr).c_str());

	for (;;) {
		char c;
		if (read(_sd, &c, 1) != 1)
			break;
		write(_sd, &c, 1);
	}

	goto close;

error:
	_errno = errno;
close:
	close(_sd);
	_sd = -1;
}
