#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <openssl/evp.h>
#include "packet.h"

class Handler;

class Client
{
public:
	Client();
	void disconnect(int e);
	void packet(const pkt_t *v);
	void handler(Handler *h) {hdr = h;}
	void encrypt(pkt_t *pkt);
	void decrypt(pkt_t *pkt);
	uint8_t decrypt(uint8_t c);

	bool isCompressed() const {return compressed;}
	bool isEncrypted() const {return encrypted;}

private:
	void handshake(const Packet *p);
	void status(const Packet *p);
	void login(const Packet *p);

	void tokengen();
	bool initEncryption();

	Handler *hdr;
	EVP_CIPHER_CTX enc, dec;

	enum {Handshake = 0, Status, Login, Play} state;
	bool compressed, encrypted;
	int32_t _proto;
	std::string _playerName;
	std::vector<uint8_t> _token, _secret;
};

#endif // CLIENT_H
