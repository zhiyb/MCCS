#pragma once

#include <string>
#include <unordered_set>
#include <stdint.h>
#include "types.h"

class Client;

class Player
{
public:
	Player(Client *c) : c(c) {}
	bool init(const std::string &name);
	void spawn();
	void tick();
	void updateChunks();
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
	void sendChunk(const chunkCoord_t &ck);
	void unloadChunk(const chunkCoord_t &ck);
	inline void sendSpawnPosition();
	inline void sendAbilities();
	inline void sendPositionLook();
	inline void sendTPPositionLook();

	void teleport(const dPosition_t &pos, const fLook_t &look);

	Client *c;

	bool _init;
	eid_t _eid;
	std::string _name, _locale;
	uint8_t _gameMode;	// enum: Gamemode
	int32_t _dimension;	// enum: Dimension
	int _chatMode;		// enum: ChatMode
	uint8_t _abilities;	// enum: Abilities
	uint8_t _view;		// View distance
	uint8_t _skin;		// enum: SkinParts
	int _mainHand;		// enum: MainHand
	iPosition_t _spawnPos;

	std::unordered_set<chunkCoord_t> chunks;

	struct {
		dPosition_t pos;
		fLook_t look;
		bool onGround;
	} server, client;

	struct {
		bool pending;
		dPosition_t pos;
		fLook_t look;
	} tp;

	struct {
		float flying;
		float move;
	} speed;
};
