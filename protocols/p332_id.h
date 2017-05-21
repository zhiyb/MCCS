#ifndef PROTOCOL_332_ID_H
#define PROTOCOL_332_ID_H

namespace Protocol {
namespace p332 {

namespace ID {
namespace Server{	// Server bound
enum {
	TPConfirm = 0, PreCraft, TabCmpl, ChatMsg, ClientStat, ClientSettings,
	Confirm, Enchant, ClickW, CloseW, PluginMsg, UseEntity,
	KeepAlive, Player, PlayerPos, PlayerPosLook, PlayerLook, VehicleMove,
	SteerBoat, PlayerAbility, PlayerDig, EntityAct, SteerVehicle, CraftBook,
	ResPackStat, AdvTab, HeldUpd, CrtvInvAct, UpdSign, Animation,
	Spectate, PlayerBlockPlace, UseItem,
};
}

namespace Client {	// Client bound
enum {
	PluginMsg = 0x18, NamedSoundEffect, Disconnect, EntityStat, Explosion,
	Unload, ChangeState, KeepAlive, Chunk, Effect, Particle, Join, Map,
	Entity, EntityMove, EntityMoveLook, EntityLook, VehicleMove, SignEdit,
	PlayerAbility, Combat, PlayerList, PlayerPosLook, UseBed, UnlockRecipes,
	DestoryEntity, RemoveEffect, ResPackSend, Respawn, EntityHeadLook,
	AdvProgress, WorldBorder, Camera, HeldChange, SBDisp, EntityMeta,
	AttachEntity, EntityVelocity, EntityEquip, SetExp, UpdHealth, SBObj,
	SetPassengers, Teams, UpdScore, SpawnPos, TimeUpd, Title, SoundEffect,
	PlayerListHF, Collect, EntityTP, Advancements, EntityProp, EntityEffect,
};
}
}

}
}

#endif
