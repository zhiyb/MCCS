#pragma once

#include <string>
#include <unordered_map>

namespace Protocol {

// Packet IDs used in actual packets
typedef int32_t pktid_t;
// Protocol IDs used internally
typedef int32_t id_t;
// Status defined below
typedef int state_t;

// Direction: server/client bound
namespace Bound {
	enum {Server = 0, Client};
}

// Client states
namespace State {
	enum {Handshake = 0, Login, Status, Play, TotalNum};
}

class Protocols
{
public:
	bool load();

	pktid_t convert(int version, state_t state, bool bound, id_t id) const
	{
		const std::unordered_map<id_t, pktid_t> *map;
		try {
			map = &mapping.at(version).pktid[state][bound];
		} catch (const std::exception &) {
			map = &mapping.at(fuzzyVersion(version)).pktid[state][bound];
		}
		try {
			return map->at(id);
		} catch (const std::exception &) {
			return -hash(version, state, bound, id);
		}
	}

	id_t resolve(int version, state_t state, bool bound, pktid_t id, bool fuzzy = false) const
	{
		const std::unordered_map<pktid_t, id_t> *map;
		try {
			map = &mapping.at(version).id[state][bound];
		} catch (const std::exception &) {
			map = &mapping.at(fuzzyVersion(version)).id[state][bound];
		}
		try {
			return map->at(id);
		} catch (const std::exception &) {
			return -hash(version, state, bound, id);
		}
	}

	int versions() const {return mapping.size();}
	bool hasVersion(int version) const {return mapping.find(version) != mapping.end();}
	int fuzzyVersion(int version) const;
	int versionMax() const;
	std::string versionString() const;

	static state_t strToState(const std::string &s);
	static int strToBound(const std::string &s);
	static int strToID(const std::string &s);
	static int32_t hash(int version, state_t state, bool bound, id_t id);
	std::string hashToStr(int32_t v, bool map = true) const;
	static std::string stateToStr(state_t v);
	static std::string boundToStr(int v);

private:
	bool load(int version, const std::string &path);
	void loadMap();

	struct map_t {
		std::unordered_map<pktid_t, id_t> id[State::TotalNum][2];
		std::unordered_map<id_t, pktid_t> pktid[State::TotalNum][2];
	};
	std::unordered_map<int, map_t> mapping;
	std::unordered_map<std::string, id_t> idMap[State::TotalNum][2];
};

extern Protocols protocols;

}
