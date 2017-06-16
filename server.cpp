#include "server.h"
#include "logging.h"
#include "player.h"

using namespace std;

void Server::tick(uint32_t t)
{
	if (t > 1)
		logger->warn("Skipping {} ticks", t - 1);
	tick(lck.players, _players);
}
