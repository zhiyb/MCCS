#pragma once

#include <unordered_set>
#include <mutex>
#include <stdint.h>

class Player;

class Server
{
public:
	void tick(uint32_t t);

	void registerItem(Player *p) {registerTo(lck.players, _players, p);}
	void unregisterItem(Player *p) {unregisterFrom(lck.players, _players, p);}

private:
	template <typename T> void tick(std::mutex &lck, std::unordered_set<T *> &set);
	template <typename T> void registerTo(std::mutex &lck, std::unordered_set<T *> &set, T *ptr);
	template <typename T> void unregisterFrom(std::mutex &lck, std::unordered_set<T *> &set, T *ptr);

	struct {
		std::mutex players;
	} lck;
	std::unordered_set<Player *> _players;
};

extern Server *server;

template <typename T> void Server::tick(std::mutex &lck, std::unordered_set<T *> &set)
{
	std::unique_lock<std::mutex> locker(lck);
	for (auto o: set)
		o->tick();
}

template <typename T> void Server::registerTo(std::mutex &lck, std::unordered_set<T *> &set, T *ptr)
{
	std::unique_lock<std::mutex> locker(lck);
	set.insert(ptr);
}

template <typename T> void Server::unregisterFrom(std::mutex &lck, std::unordered_set<T *> &set, T *ptr)
{
	std::unique_lock<std::mutex> locker(lck);
	set.erase(ptr);
}
