#include <string>
#include <sstream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include "status.h"

using std::string;
using std::stringstream;

Status status;

string Status::version() const
{
	return "1.12-pre5";
}

int Status::protocol() const
{
	return 332;
}

int Status::playersMax() const
{
	return 128;
}

std::string Status::description() const
{
	return "Minecraft custom server (work in progress)";
}

string Status::toJson() const
{
	using namespace rapidjson;
	Document d;
	d.SetObject();
	auto &a = d.GetAllocator();

	Value ver;
	ver.SetObject();
	string vname = version();
	ver.AddMember("name", Value(vname.c_str(), vname.length(), a), a);
	ver.AddMember("protocol", protocol(), a);
	d.AddMember("version", ver, a);

	Value player;
	player.SetObject();
	player.AddMember("max", playersMax(), a);
	player.AddMember("online", playersOnline(), a);
	d.AddMember("players", player, a);

	Value desc;
	desc.SetObject();
	string dtext = description();
	desc.AddMember("text", Value(dtext.c_str(), dtext.length(), a), a);
	d.AddMember("description", desc, a);

	stringstream ss;
	OStreamWrapper osw(ss);
	Writer<OStreamWrapper> writer(osw);
	d.Accept(writer);
	return ss.str();
}
