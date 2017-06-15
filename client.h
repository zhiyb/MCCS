#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <openssl/evp.h>
#include "protocols.h"
#include "packet.h"
#include "logging.h"

class Handler;

class Client
{
public:
	Client();
	void handler(Handler *h) {hdr = h;}

	void packet(pkt_t *v);
	void keepAlive();
	void logDisconnect(int e);

	void encrypt(pkt_t *pkt);
	void encryptAppend(pkt_t *src, pkt_t *dst);
	void decrypt(pkt_t *pkt);
	uint8_t decrypt(uint8_t c);

	bool isCompressed() const {return compressed;}
	bool isEncrypted() const {return encrypted;}

private:
	void handshake(const Packet *p);
	void status(const Packet *p);
	void login(const Packet *p);

	void playInit();
	void play(const Packet *p);

	void sendNewChunks(double x, double z);
	void sendNewChunk(int32_t x, int32_t z);
	void pushChunkSection(pkt_t *p, int32_t x, int32_t z, bool biome);

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

	Protocol::state_t state;
	bool compressed, encrypted;
	int32_t _protocol, _version, _keepAlive;
	std::string _playerName;
	std::vector<uint8_t> _token, _secret;
};

#endif // CLIENT_H
