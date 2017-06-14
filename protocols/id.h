#pragma once

namespace Protocol {

// IDs used in Handshake state
namespace Handshake {
namespace Client {
}
namespace Server {
	enum {Handshake};
}
}

// IDs used in Login state
namespace Login {
namespace Client {
	enum {Disconnect, Encryption, Success, Compression};
}
namespace Server {
	enum {Start, Encryption};
}
}

// IDs used in Status state
namespace Status {
namespace Client {
	enum {Response, Pong};
}
namespace Server {
	enum {Request, Ping};
}
}

// IDs used in Play state
namespace Play {
namespace Client {
	enum {SpawnObject, SpawnExpOrb, SpawnGlobalEntity, SpawnMob, SpawnPainting, SpawnPlayer,
		Animation, Statistics, BlockBreakAnimation, UpdateBlockEntity,
		BlockAction, BlockChange, BossBar, ServerDifficulty, TabComplete, ChatMessage, MultiBlockChange, Confirm,
		CloseWindow, OpenWindow, WindowItems, WindowProperty, SetSlot, SetCooldown,
		PluginMessage, NamedSoundEffect, Disconnect, EntityStatus, Explosion,
		UnloadChunk, ChangeGameState, KeepAlive, ChunkData, Effect, Particle,
		JoinGame, Map, Entity, EntityMove, EntityLookMove, EntityLook,
		VehicleMove, OpenSignEditor, PlayerAbilities, CombatEvent,
		PlayerListItem, PlayerPositionLook, UseBed, UnlockRecipes,
		DestroyEntities, RemoveEntityEffect, ResourcePackSend,
		Respawn, EntityHeadLook, SelectAdvancementTab, WorldBorder,
		Camera, HeldItemChange, DisplayScoreboard,
		EntityMetadata, AttachEntity, EntityVelocity, EntityEquipment,
		SetExp, UpdateHealth, ScoreboardObjective, SetPassengers,
		Teams, UpdateScore, SpawnPosition, TimeUpdate, Title,
		SoundEffect, PlayerListHeaderFooter, CollectItem,
		EntityTeleport, Advancements, EntityProperties, EntityEffect};
}
namespace Server {
	enum {TeleportConfirm, PrepareCraftingGrid, TabComplete, ChatMessage,
		ClientStatus, ClientSettings, ConfirmTransaction, EnchantItem,
		ClickWindow, CloseWindow, PluginMessage, UseEntity, KeepAlive,
		Player, PlayerPosition, PlayerPositionLook, PlayerLook,
		VehicleMove, SteerBoat, PlayerAbilities, PlayerDigging,
		EntityAction, SteerVehicle, CraftingBookData, ResourcePackStatus,
		AdvancementTab, HeldItemChange, CreativeInventoryAction,
		UpdateSign, Animation, Spectate, PlayerBlockPlacement, UseItem};
}
}

// ID lists with descriptions
#ifdef PROTOCOL_ENUM_LIST
static const struct {
	state_t state;
	bool bound;
	id_t id;
	const char *type;
} id_list[] = {
	{State::Handshake, Bound::Server, Handshake::Server::Handshake, "Handshake"},
	{State::Login, Bound::Client, Login::Client::Disconnect, "Disconnect"},
	{State::Login, Bound::Client, Login::Client::Encryption, "Encryption"},
	{State::Login, Bound::Client, Login::Client::Success, "Success"},
	{State::Login, Bound::Client, Login::Client::Compression, "Compression"},
	{State::Login, Bound::Server, Login::Server::Start, "Start"},
	{State::Login, Bound::Server, Login::Server::Encryption, "Encryption"},
	{State::Status, Bound::Client, Status::Client::Response, "Response"},
	{State::Status, Bound::Client, Status::Client::Pong, "Pong"},
	{State::Status, Bound::Server, Status::Server::Request, "Request"},
	{State::Status, Bound::Server, Status::Server::Ping, "Ping"},

	{State::Play, Bound::Client, Play::Client::SpawnObject, "SpawnObject"},
	{State::Play, Bound::Client, Play::Client::SpawnExpOrb, "SpawnExpOrb"},
	{State::Play, Bound::Client, Play::Client::SpawnGlobalEntity, "SpawnGlobalEntity"},
	{State::Play, Bound::Client, Play::Client::SpawnMob, "SpawnMob"},
	{State::Play, Bound::Client, Play::Client::SpawnPainting, "SpawnPainting"},
	{State::Play, Bound::Client, Play::Client::SpawnPlayer, "SpawnPlayer"},
	{State::Play, Bound::Client, Play::Client::Animation, "Animation"},
	{State::Play, Bound::Client, Play::Client::Statistics, "Statistics"},
	{State::Play, Bound::Client, Play::Client::BlockBreakAnimation, "BlockBreakAnimation"},
	{State::Play, Bound::Client, Play::Client::UpdateBlockEntity, "UpdateBlockEntity"},
	{State::Play, Bound::Client, Play::Client::BlockAction, "BlockAction"},
	{State::Play, Bound::Client, Play::Client::BlockChange, "BlockChange"},
	{State::Play, Bound::Client, Play::Client::BossBar, "BossBar"},
	{State::Play, Bound::Client, Play::Client::ServerDifficulty, "ServerDifficulty"},
	{State::Play, Bound::Client, Play::Client::TabComplete, "TabComplete"},
	{State::Play, Bound::Client, Play::Client::ChatMessage, "ChatMessage"},
	{State::Play, Bound::Client, Play::Client::MultiBlockChange, "MultiBlockChange"},
	{State::Play, Bound::Client, Play::Client::Confirm, "Confirm"},
	{State::Play, Bound::Client, Play::Client::CloseWindow, "CloseWindow"},
	{State::Play, Bound::Client, Play::Client::OpenWindow, "OpenWindow"},
	{State::Play, Bound::Client, Play::Client::WindowItems, "WindowItems"},
	{State::Play, Bound::Client, Play::Client::WindowProperty, "WindowProperty"},
	{State::Play, Bound::Client, Play::Client::SetSlot, "SetSlot"},
	{State::Play, Bound::Client, Play::Client::SetCooldown, "SetCooldown"},
	{State::Play, Bound::Client, Play::Client::PluginMessage, "PluginMessage"},
	{State::Play, Bound::Client, Play::Client::NamedSoundEffect, "NamedSoundEffect"},
	{State::Play, Bound::Client, Play::Client::Disconnect, "Disconnect"},
	{State::Play, Bound::Client, Play::Client::EntityStatus, "EntityStatus"},
	{State::Play, Bound::Client, Play::Client::Explosion, "Explosion"},
	{State::Play, Bound::Client, Play::Client::UnloadChunk, "UnloadChunk"},
	{State::Play, Bound::Client, Play::Client::ChangeGameState, "ChangeGameState"},
	{State::Play, Bound::Client, Play::Client::KeepAlive, "KeepAlive"},
	{State::Play, Bound::Client, Play::Client::ChunkData, "ChunkData"},
	{State::Play, Bound::Client, Play::Client::Effect, "Effect"},
	{State::Play, Bound::Client, Play::Client::Particle, "Particle"},
	{State::Play, Bound::Client, Play::Client::JoinGame, "JoinGame"},
	{State::Play, Bound::Client, Play::Client::Map, "Map"},
	{State::Play, Bound::Client, Play::Client::Entity, "Entity"},
	{State::Play, Bound::Client, Play::Client::EntityMove, "EntityMove"},
	{State::Play, Bound::Client, Play::Client::EntityLookMove, "EntityLookMove"},
	{State::Play, Bound::Client, Play::Client::EntityLook, "EntityLook"},
	{State::Play, Bound::Client, Play::Client::VehicleMove, "VehicleMove"},
	{State::Play, Bound::Client, Play::Client::OpenSignEditor, "OpenSignEditor"},
	{State::Play, Bound::Client, Play::Client::PlayerAbilities, "PlayerAbilities"},
	{State::Play, Bound::Client, Play::Client::CombatEvent, "CombatEvent"},
	{State::Play, Bound::Client, Play::Client::PlayerListItem, "PlayerListItem"},
	{State::Play, Bound::Client, Play::Client::PlayerPositionLook, "PlayerPositionLook"},
	{State::Play, Bound::Client, Play::Client::UseBed, "UseBed"},
	{State::Play, Bound::Client, Play::Client::UnlockRecipes, "UnlockRecipes"},
	{State::Play, Bound::Client, Play::Client::DestroyEntities, "DestroyEntities"},
	{State::Play, Bound::Client, Play::Client::RemoveEntityEffect, "RemoveEntityEffect"},
	{State::Play, Bound::Client, Play::Client::ResourcePackSend, "ResourcePackSend"},
	{State::Play, Bound::Client, Play::Client::Respawn, "Respawn"},
	{State::Play, Bound::Client, Play::Client::EntityHeadLook, "EntityHeadLook"},
	{State::Play, Bound::Client, Play::Client::SelectAdvancementTab, "SelectAdvancementTab"},
	{State::Play, Bound::Client, Play::Client::WorldBorder, "WorldBorder"},
	{State::Play, Bound::Client, Play::Client::Camera, "Camera"},
	{State::Play, Bound::Client, Play::Client::HeldItemChange, "HeldItemChange"},
	{State::Play, Bound::Client, Play::Client::DisplayScoreboard, "DisplayScoreboard"},
	{State::Play, Bound::Client, Play::Client::EntityMetadata, "EntityMetadata"},
	{State::Play, Bound::Client, Play::Client::AttachEntity, "AttachEntity"},
	{State::Play, Bound::Client, Play::Client::EntityVelocity, "EntityVelocity"},
	{State::Play, Bound::Client, Play::Client::EntityEquipment, "EntityEquipment"},
	{State::Play, Bound::Client, Play::Client::SetExp, "SetExp"},
	{State::Play, Bound::Client, Play::Client::UpdateHealth, "UpdateHealth"},
	{State::Play, Bound::Client, Play::Client::ScoreboardObjective, "ScoreboardObjective"},
	{State::Play, Bound::Client, Play::Client::SetPassengers, "SetPassengers"},
	{State::Play, Bound::Client, Play::Client::Teams, "Teams"},
	{State::Play, Bound::Client, Play::Client::UpdateScore, "UpdateScore"},
	{State::Play, Bound::Client, Play::Client::SpawnPosition, "SpawnPosition"},
	{State::Play, Bound::Client, Play::Client::TimeUpdate, "TimeUpdate"},
	{State::Play, Bound::Client, Play::Client::Title, "Title"},
	{State::Play, Bound::Client, Play::Client::SoundEffect, "SoundEffect"},
	{State::Play, Bound::Client, Play::Client::PlayerListHeaderFooter, "PlayerListHeaderFooter"},
	{State::Play, Bound::Client, Play::Client::CollectItem, "CollectItem"},
	{State::Play, Bound::Client, Play::Client::EntityTeleport, "EntityTeleport"},
	{State::Play, Bound::Client, Play::Client::Advancements, "Advancements"},
	{State::Play, Bound::Client, Play::Client::EntityProperties, "EntityProperties"},
	{State::Play, Bound::Client, Play::Client::EntityEffect, "EntityEffect"},

	{State::Play, Bound::Server, Play::Server::TeleportConfirm, "TeleportConfirm"},
	{State::Play, Bound::Server, Play::Server::PrepareCraftingGrid, "PrepareCraftingGrid"},
	{State::Play, Bound::Server, Play::Server::TabComplete, "TabComplete"},
	{State::Play, Bound::Server, Play::Server::ChatMessage, "ChatMessage"},
	{State::Play, Bound::Server, Play::Server::ClientStatus, "ClientStatus"},
	{State::Play, Bound::Server, Play::Server::ClientSettings, "ClientSettings"},
	{State::Play, Bound::Server, Play::Server::ConfirmTransaction, "ConfirmTransaction"},
	{State::Play, Bound::Server, Play::Server::EnchantItem, "EnchantItem"},
	{State::Play, Bound::Server, Play::Server::ClickWindow, "ClickWindow"},
	{State::Play, Bound::Server, Play::Server::CloseWindow, "CloseWindow"},
	{State::Play, Bound::Server, Play::Server::PluginMessage, "PluginMessage"},
	{State::Play, Bound::Server, Play::Server::UseEntity, "UseEntity"},
	{State::Play, Bound::Server, Play::Server::KeepAlive, "KeepAlive"},
	{State::Play, Bound::Server, Play::Server::Player, "Player"},
	{State::Play, Bound::Server, Play::Server::PlayerPosition, "PlayerPosition"},
	{State::Play, Bound::Server, Play::Server::PlayerPositionLook, "PlayerPositionLook"},
	{State::Play, Bound::Server, Play::Server::PlayerLook, "PlayerLook"},
	{State::Play, Bound::Server, Play::Server::VehicleMove, "VehicleMove"},
	{State::Play, Bound::Server, Play::Server::SteerBoat, "SteerBoat"},
	{State::Play, Bound::Server, Play::Server::PlayerAbilities, "PlayerAbilities"},
	{State::Play, Bound::Server, Play::Server::PlayerDigging, "PlayerDigging"},
	{State::Play, Bound::Server, Play::Server::EntityAction, "EntityAction"},
	{State::Play, Bound::Server, Play::Server::SteerVehicle, "SteerVehicle"},
	{State::Play, Bound::Server, Play::Server::CraftingBookData, "CraftingBookData"},
	{State::Play, Bound::Server, Play::Server::ResourcePackStatus, "ResourcePackStatus"},
	{State::Play, Bound::Server, Play::Server::AdvancementTab, "AdvancementTab"},
	{State::Play, Bound::Server, Play::Server::HeldItemChange, "HeldItemChange"},
	{State::Play, Bound::Server, Play::Server::CreativeInventoryAction, "CreativeInventoryAction"},
	{State::Play, Bound::Server, Play::Server::UpdateSign, "UpdateSign"},
	{State::Play, Bound::Server, Play::Server::Animation, "Animation"},
	{State::Play, Bound::Server, Play::Server::Spectate, "Spectate"},
	{State::Play, Bound::Server, Play::Server::PlayerBlockPlacement, "PlayerBlockPlacement"},
	{State::Play, Bound::Server, Play::Server::UseItem, "UseItem"},
};
#endif

}
