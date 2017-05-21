#ifndef PROTOCOL_332_ID_H
#define PROTOCOL_332_ID_H

namespace Protocol {
namespace p332 {

namespace ID {
enum {
	// Server bound
	PreCraft = 0x01, TabCmpl, ChatMsg, ClientStat, ClientSettings,
	Confirm, Enchant, ClickW, CloseW, PluginMsg, UseEntity,
	KeepAlive, Player, PlayerPos, PlayerPosLook, PlayerLook, VehicleMove,
	SteerBoat, PlayerAbility, PlayerDig, EntityAct, SteerVehicle, CraftBook,
	ResPackStat, AdvTab, HeldUpd, CrtvInvAct, UpdSign, Animation,
	Spectate, PlayerBlockPlace, UseItem,
	// Client bound
	Entity = 0x25, EntityMove, EntityMoveLook, EntityLook, Unlock = 0x30,
	DestoryEntity, RemoveEffect, ResPackSend, Respawn, EntityHeadLook,
	AdvProgress, WorldBorder, Camera, HeldChange, SBDisp, EntityMeta,
	AttachEntity, EntityVelocity, EntityEquip, SetExp, UpdHealth, SBObj,
	SetPassengers, Teams, UpdScore, SpawnPos, TimeUpd, Title, SoundEffect,
	PlayerListHF, Collect, EntityTP, Advancements, EntityProp, EntityEffect,
};
}

}
}

#endif
