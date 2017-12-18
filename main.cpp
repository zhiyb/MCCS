#include <string.h>
#include <openssl/err.h>
#include "network.h"
#include "status.h"
#include "protocols.h"
#include "logging.h"
#include "tick.h"
#include "server.h"

using namespace Protocol;

std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("console");
Server *server;

int main(int argc, char *argv[])
{
	//logger->set_pattern("[%Y-%m-%d %T t%t] [%l] %v");
	logger->set_pattern("[%T %t/%l]: %v");
	logger->set_level(spdlog::level::debug);

	if (status.keygen() != 0) {
		logger->error("Key generation failed: {}", ERR_get_error());
		return 1;
	}

	protocols.load();
	if (protocols.versions() == 0) {
		logger->error("No protocol versions available");
		return 1;
	}

	Network n;
	if (n.err() != 0) {
		logger->error("Network initilisation failed");
		return 1;
	}
	if (!n.listen()) {
		logger->error("Cannot listen on {}", n.host().c_str());
		return 1;
	}
	logger->info("Listening on {}", n.host().c_str());

	Server s;
	server = &s;

	Tick t;
	t.start();

	if (!n.process())
		logger->error("Network process failed: {}", strerror(n.err()));

	logger->info("Shutting down...");
	t.stop();
	n.close();
	return 0;
}
