#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <openssl/evp.h>
#include "protocols.h"
#include "packet.h"
#include "types.h"

namespace Chat {
	class Message;
}

class Handler;
class Player;

class Client
{
public:
	Client();
	~Client();
	void handler(Handler *h) {hdr = h;}
	Handler *handler() {return hdr;}

	void packet(pkt_t *v);
	void keepAlive();
	void logDisconnect(int e);

	void encrypt(pkt_t *pkt);
	void encryptAppend(pkt_t *src, pkt_t *dst);
	void decrypt(pkt_t *pkt);
	uint8_t decrypt(uint8_t c);

	bool isCompressed() const {return compressed;}
	bool isEncrypted() const {return encrypted;}

	void join(eid_t eid, uint8_t gameMode, int32_t dimension, uint8_t difficulty, std::string level, bool debug) const;
	void spawnPosition(const iPosition_t &position) const;
	void playerAbilities(uint8_t abilities, float speed, float fov) const;
	void playerPositionLook(const dPosition_t &pos, const fLook_t &look, uint8_t relative);
	void disconnectPlayer(const Chat::Message &reason);

	void sendNewChunks(double x, double z);
	void sendNewChunk(int32_t x, int32_t z);
	void pushChunkSection(pkt_t *p, int32_t x, int32_t z, bool biome);

private:
	void handshake(const Packet *p);
	void status(const Packet *p);
	void login(const Packet *p);

	void playInit();
	void play(const Packet *p);

	Protocol::id_t id(Protocol::pktid_t i) const
	{
		return Protocol::protocols.resolve(_protocol, state, Protocol::Bound::Server, i);
	}

	Protocol::pktid_t pktid(Protocol::id_t i) const
	{
		return Protocol::protocols.convert(_protocol, state, Protocol::Bound::Client, i);
	}

	void tokengen();
	bool initEncryption();

	EVP_CIPHER_CTX enc, dec;

	Handler *hdr;
	Player *player;

	Protocol::state_t state;
	bool compressed, encrypted;
	int32_t _protocol, _version, _keepAlive, _tpID;
	std::string _playerName, _reason;
	std::vector<uint8_t> _token, _secret;
};

#endif // CLIENT_H
