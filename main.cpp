#include <syslog.h>
#include <string.h>
#include <iostream>
#include "network.h"

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
	openlog("MCCS", LOG_PERROR, LOG_USER);

	Network n;
	if (!n.listen()) {
		syslog(LOG_ERR, "Cannot listen on %s: %s\n", n.host().c_str(), strerror(n.err()));
		return 1;
	}
	syslog(LOG_INFO, "Listening on %s\n", n.host().c_str());

	if (!n.process()) {
		syslog(LOG_ERR, "Network process failed: %s\n", strerror(n.err()));
		goto close;
	}

close:
	syslog(LOG_INFO, "Closing socket...\n");
	n.close();
	return 0;
}
