#include "player.h"
#include "client.h"
#include "logging.h"
#include "chat.h"
#include "types.h"

using namespace std;

inline void Player::sendSpawnPosition() {c->spawnPosition(_spawnPos);}
inline void Player::sendAbilities() {c->playerAbilities(_abilities, speed.flying, speed.move);}
inline void Player::sendPositionLook() {c->playerPositionLook(_pos, _look, 0);}
inline void Player::sendTPPositionLook() {c->playerPositionLook(_tpPos, _tpLook, 0);}

void Player::teleport(const dPosition_t &pos, const fLook_t &look)
{
	_tpPos = pos;
	_tpLook = look;
	sendPositionLook();
}

bool Player::init(const string &name)
{
	_name = name;
	_eid = 0;
	_spawnPos = {0, 0, 0};
	_pos = {0, 18, 0};
	_look = {0, 0};
	_onGround = false;
	_abilities = Abilities::Invulnerable | Abilities::Flying | Abilities::AllowFlying | Abilities::CreativeMode;
	speed.flying = 0.4f;
	speed.move = 0.7f;

	// EID, game mode, dimension, difficulty, level type, debug info
	c->join(_eid, Gamemode::Creative, Dimension::Overworld, Difficulty::Peaceful, "default", true);
	sendSpawnPosition();
	sendAbilities();
	sendPositionLook();
	return true;
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
	_view = d;
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
	_cPos = _tpPos;
	_cLook = _tpLook;
}

void Player::moveTo(const dPosition_t &pos)
{
	_cPos = pos;
}

void Player::lookAt(const fLook_t &look)
{
	_cLook = look;
}

void Player::onGround(bool e)
{
	_cOnGround = e;
}

void Player::disconnected(int e)
{
}
/* }}} */
