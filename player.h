#pragma once

#include <string>
#include <stdint.h>
#include "types.h"

class Client;

class Player
{
public:
	Player(Client *c) : c(c) {}
	bool init(const std::string &name);
	std::string error();

	void locale(const std::string &locale);
	void viewDistance(uint8_t d);
	void chatMode(int mode);
	void skinDisplay(uint8_t mode);
	void mainHand(int mode);

	void teleportConfirm();
	void moveTo(const dPosition_t &pos);
	void lookAt(const fLook_t &look);
	void onGround(bool e);
	void disconnected(int e);

private:
	inline void sendSpawnPosition();
	inline void sendAbilities();
	inline void sendPositionLook();
	inline void sendTPPositionLook();

	void teleport(const dPosition_t &pos, const fLook_t &look);

	Client *c;

	eid_t _eid;
	std::string _name, _locale;
	int _chatMode;		// enum: ChatMode
	uint8_t _abilities;	// enum: Abilities
	uint8_t _view;		// View distance
	uint8_t _skin;		// enum: SkinParts
	int _mainHand;		// enum: MainHand
	iPosition_t _spawnPos;
	dPosition_t _pos, _cPos, _tpPos;
	fLook_t _look, _cLook, _tpLook;
	bool _onGround, _cOnGround;

	struct {
		float flying;
		float move;
	} speed;
};
