#pragma once

#include <stdint.h>

static const uint32_t fp = 1u << 5;

enum Gamemode {Survival = 0, Creative, Adventure, Spectator, Hardcore = 0x8};
enum Dimension {Nether = -1, Overworld = 0, End = 1};
enum Difficulty {Peaceful = 0, Easy, Normal, Hard};
enum Abilities {Invulnerable = 0x01, Flying = 0x02, AllowFlying = 0x04, CreativeMode = 0x08};
enum Relative {X = 0x01, Y = 0x02, Z = 0x04, Y_ROT = 0x08, X_ROT = 0x10};
enum ChatMode {Enabled = 0, Commands = 1, Hidden = 2, Colors = 0x80};
enum SkinParts {Cape = 0x01, Jacket = 0x02, LeftSleeve = 0x04, RightSleeve = 0x08, LeftLeg = 0x10, RightLeg = 0x20, Hat = 0x40};
enum MainHand {Left = 0, Right = 1};

typedef int32_t eid_t;

typedef struct iPosition_t {
	uint32_t x, z;
	uint16_t y;
} iPosition_t;

typedef struct dPosition_t {
	double x, y, z;
} dPosition_t;

typedef struct fLook_t {
	float yaw, pitch;
} fLook_t;
