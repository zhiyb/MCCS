#include "player.h"
#include "client.h"
#include "server.h"
#include "status.h"
#include "logging.h"
#include "chat.h"
#include "types.h"

using namespace std;

void Player::teleport(const dPosition_t &pos, const fLook_t &look)
{
	tp.pos = pos;
	tp.look = look;
	tp.pending = true;
	sendPositionLook();
}

bool Player::init(const string &name)
{
	_init = true;
	_eid = 0;
	_name = name;
	_gameMode = Gamemode::Creative;
	_dimension = Dimension::Overworld;
	_spawnPos = {0, 0, 0};
	_abilities = Abilities::Invulnerable | Abilities::Flying | Abilities::AllowFlying | Abilities::CreativeMode;
	speed.flying = 0.4f;
	speed.move = 0.7f;
	server.pos = {0, 18, 0};
	server.look = {0, 0};
	server.onGround = false;
	client = server;
	tp.pending = false;

	c->join(_eid, _gameMode, _dimension, status.difficulty(), status.levelType(), status.debug());
	sendSpawnPosition();
	sendAbilities();
	return true;
}

void Player::spawn()
{
	if (_init)
		::server->registerItem(this);
}

inline void Player::sendSpawnPosition() {c->spawnPosition(_spawnPos);}
inline void Player::sendAbilities() {c->playerAbilities(_abilities, speed.flying, speed.move);}
inline void Player::sendPositionLook() {c->playerPositionLook(server.pos, server.look, 0);}
inline void Player::sendTPPositionLook() {c->playerPositionLook(tp.pos, tp.look, 0);}

void Player::sendChunk(const chunkCoord_t &ck)
{
	c->sendNewChunk(ck);
	chunks.insert(ck);
}

void Player::unloadChunk(const chunkCoord_t &ck)
{
	c->unloadChunk(ck);
	chunks.erase(ck);
}

void Player::tick()
{
	if (tp.pending)
		return;
	if (_init) {
		chunkCoord_t sChunk = positionToChunkCoord(server.pos);
		chunkCoord_t iChunk = sChunk - chunkCoord_t({3, 3});
		for (int i = 0; i != 7; i++)
			for (int j = 0; j != 7; j++)
				sendChunk(iChunk + chunkCoord_t({i, j}));
		teleport(server.pos, server.look);
		_init = false;
		return;
	}
	server.pos = client.pos;
	server.look = client.look;
	server.onGround = client.onGround;
	updateChunks();
}

void Player::updateChunks()
{
	// Calculate chunks to be unloaded (distance > sqrt(2) * view)
	chunkCoord_t chunk = positionToChunkCoord(server.pos);
	std::unordered_set<chunkCoord_t> unloads;
	for (const auto &ck: chunks) {
		chunkCoord_t d = chunk - ck;
		if (d.x * d.x + d.z * d.z > _view * _view * 2)
			unloads.insert(ck);
	}

	// Send nearby chunk data
	for (const auto &c: chunksNearby) {
		chunkCoord_t ck = chunk + c;
		if (chunks.find(ck) == chunks.end())
			sendChunk(ck);
	}

	// Unload chunks
	for (const auto &ck: unloads)
		unloadChunk(ck);
}

string Player::error()
{
	return "Player::Unknown";
}

/* {{{ Client settings */
void Player::locale(const string &locale)
{
	_locale = locale;
}

void Player::viewDistance(uint8_t d)
{
	if (d == _view)
		return;
	_view = d;
	// Recalculate nearby chunk coordinates
	typedef tuple<int32_t, int32_t, int32_t> data_t;
	vector<data_t> dist;
	for (int32_t x = -d; x != d + 1; x++)
		for (int32_t z = -d; z != d + 1; z++)
			dist.push_back(make_tuple(x, z, d * d - x * x - z * z));
	sort(dist.begin(), dist.end(), [](const data_t &a, const data_t &b) {return get<2>(a) > get<2>(b);});
	chunksNearby.clear();
	for (auto &d: dist)
		if (get<2>(d) >= 0)
			chunksNearby.push_back({get<0>(d), get<1>(d)});
}

void Player::chatMode(int mode)
{
	_chatMode = mode;
}

void Player::skinDisplay(uint8_t mode)
{
	_skin = mode;
}

void Player::mainHand(int mode)
{
	_mainHand = mode;
}
/* }}} */

/* {{{ Client update */
void Player::teleportConfirm()
{
	client.pos = tp.pos;
	client.look = tp.look;
	tp.pending = false;
}

void Player::moveTo(const dPosition_t &pos)
{
	client.pos = pos;
}

void Player::lookAt(const fLook_t &look)
{
	client.look = look;
}

void Player::onGround(bool e)
{
	client.onGround = e;
}

void Player::disconnected(int e)
{
	::server->unregisterItem(this);
}
/* }}} */
