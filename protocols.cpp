#include <exception>
#include <string>
#include <regex>
#include <sstream>
#include <climits>
#include <dirent.h>
#include <csv.h>
#include "logging.h"
#include "protocols.h"
#define PROTOCOL_ENUM_LIST
#include "protocols/id.h"

using namespace std;
using namespace Protocol;

Protocols Protocol::protocols;

bool Protocols::load()
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("protocols")) == NULL)
		return false;
	std::vector<int> versions;
	while ((ent = readdir(dir)) != NULL) {
		if (!regex_match(ent->d_name, regex("^[0-9]+.*.csv$")))
			continue;
		try {
			versions.push_back(stoi(ent->d_name));
		} catch (const exception &) {
			continue;
		}
	}
	sort(versions.begin(), versions.end());
	loadMap();
	bool fail = true;
	for (auto ver: versions) {
		stringstream ss;
		ss << "protocols/" << ver << ".csv";
		// TODO: Copy previous loading results for incremental design
		fail = load(ver, ss.str()) ?: fail;
	}
	return fail;
}

int Protocols::fuzzyVersion(int version) const
{
	if (version == 0)
		return versionMax();
	int lower = 0, upper = INT_MAX;
	for (auto it: mapping) {
		if (version == it.first)
			return it.first;
		else if (version > it.first)
			lower = it.first > lower ? it.first : lower;
		else if (version < it.first)
			upper = it.first < upper ? it.first : upper;
	}
	if (lower != 0)
		return lower;
	return upper;
}

int Protocols::versionMax() const
{
	int max = 0;
	for (auto it: mapping)
		max = max < it.first ? it.first : max;
	return max;
}

std::string Protocols::versionString() const
{
	int min = 0, max = 0;
	for (auto it: mapping) {
		if (min == 0) {
			min = it.first;
			max = it.first;
		} else {
			min = min > it.first ? it.first : min;
			max = max < it.first ? it.first : max;
		}
	}
	stringstream ss;
	ss << "p" << min;
	if (min != max)
		ss << " - p" << max;
	return ss.str();
}

bool Protocols::load(int version, const string &file)
{
	if (hasVersion(version))
		return false;
	try {
		io::CSVReader<4> in(file);
		in.read_header(io::ignore_extra_column, "State", "Bound", "ID", "Type");
		unsigned long line = 1, ids = 0;
		auto &map = mapping[version];
		string strState, strBound, strPktID, type;
		while (in.read_row(strState, strBound, strPktID, type)) {
			line++;
			state_t state = strToState(strState);
			int bound = strToBound(strBound);
			int pktid = strToID(strPktID);
			if (state == -1) {
				logger->warn("{}:{}: Unknown state {}", file, line, strState);
				continue;
			} else if (bound == -1) {
				logger->warn("{}:{}: Unknown bound {}", file, line, strBound);
				continue;
			} else if (pktid == -1) {
				logger->warn("{}:{}: Unable to interpret packet ID {}", file, line, strPktID);
				continue;
			}
			type.erase(remove(type.begin(), type.end(), ' '), type.end());
			id_t id;
			try {
				id = idMap[state][bound].at(type.c_str());
			} catch (const exception &) {
				logger->warn("{}:{}: Type {}::{}::{} does not exist", file, line,
						stateToStr(state), boundToStr(bound), type);
				continue;
			}
			map.id[state][bound][pktid] = id;
			map.pktid[state][bound][id] = pktid;
			ids++;
		}
		logger->info("Read {} packet IDs from {}", ids, file);
	} catch (const exception &e) {
		logger->warn("Error reading protocol {}: {}",
				version, e.what());
		return false;
	}
	return true;
}

void Protocols::loadMap()
{
	for (auto &id: id_list)
		idMap[id.state][id.bound][id.type] = id.id;
}

state_t Protocols::strToState(const std::string &s)
{
	if (s == "Handshake")
		return State::Handshake;
	else if (s == "Login")
		return State::Login;
	else if (s == "Status")
		return State::Status;
	else if (s == "Play")
		return State::Play;
	return -1;
}

int Protocols::strToBound(const std::string &s)
{
	if (s == "Server")
		return Bound::Server;
	else if (s == "Client")
		return Bound::Client;
	return -1;
}

int Protocols::strToID(const std::string &s)
{
	try {
		return stoi(s, 0, 0);
	} catch (const exception &) {
		return -1;
	}
}

int32_t Protocols::hash(int version, state_t state, bool bound, id_t id)
{
	return (version << 16) | (bound << 15) | (state << 8) | id;
}

std::string Protocols::hashToStr(int32_t v, bool map) const
{
	id_t id = v & 0xff;
	v >>= 8;
	state_t state = v & 0x7f;
	v >>= 7;
	bool bound = state & 0x01;
	int version = v >> 1;
	stringstream ss;
	ss << "p" << version << "::" << stateToStr(state) << "::" << boundToStr(bound) << "::";
	if (!map) {
		ss << id;
		return ss.str();
	}
	bool found = false;
	for (auto &i: id_list)
		if (i.state == state && i.bound == bound && i.id == id) {
			found = true;
			ss << i.type;
			break;
		}
	if (!found)
		ss << "Unknown" << id;
	return ss.str();
}

std::string Protocols::stateToStr(state_t v)
{
	switch (v) {
	case State::Play:
		return "Play";
	case State::Handshake:
		return "Handshake";
	case State::Login:
		return "Login";
	case State::Status:
		return "Status";
	default:
		return "Unknown";
	}
}

std::string Protocols::boundToStr(int v)
{
	switch (v) {
	case Bound::Client:
		return "Client";
	case Bound::Server:
		return "Server";
	default:
		return "Unknown";
	}
}
