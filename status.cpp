#include <string>
#include <sstream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/asn1.h>
#include <openssl/objects.h>
#include "status.h"
#include "logging.h"
#include "protocols.h"
#include "chat.h"
#include "types.h"

using std::string;
using std::stringstream;

Status status;

Status::Status()
{
	srand(time(NULL));
	rsa = 0;
	_online = 0;
	_difficulty = Difficulty::Peaceful;
	_level = "default";
	_debug = true;
}

Status::~Status()
{
	RSA_free(rsa);
}

int Status::keygen()
{
	rsa = RSA_generate_key(1024, 65537, NULL, NULL);
	if (rsa == NULL)
		return -1;
	return 0;
}

int Status::pubkey(BIO *bio)
{
	// TODO: Error checking
	i2d_RSA_PUBKEY_bio(bio, rsa);
	return 0;
}

int Status::decrypt(std::vector<uint8_t> &from, std::vector<uint8_t> &to)
{
	to.resize(RSA_size(rsa));
	int ret = RSA_private_decrypt(from.size(), from.data(),
			to.data(), rsa, RSA_PKCS1_PADDING);
	if (ret != -1)
		to.resize(ret);
	return ret;
}

string Status::version() const
{
	return Protocol::protocols.versionString();
}

int Status::protocol() const
{
	return Protocol::protocols.versionMax();
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
	Document d(kObjectType);
	auto &a = d.GetAllocator();

	Value ver(kObjectType);
	string vname = version();
	ver.AddMember("name", StringRef(vname.c_str()), a);
	ver.AddMember("protocol", protocol(), a);
	d.AddMember("version", ver, a);

	Value player(kObjectType);
	player.AddMember("max", playersMax(), a);
	player.AddMember("online", playersOnline(), a);
	d.AddMember("players", player, a);

	Value desc(kObjectType);
	Chat::Text(description()).write(desc, a);
	d.AddMember("description", desc, a);

	stringstream ss;
	OStreamWrapper osw(ss);
	Writer<OStreamWrapper> writer(osw);
	d.Accept(writer);
	return ss.str();
}
