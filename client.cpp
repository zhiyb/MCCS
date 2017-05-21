#include <string.h>
#include <syslog.h>
#include <iostream>
#include <openssl/bio.h>
#include "client.h"
#include "handler.h"
#include "status.h"
#include "packets/packets.h"

Client::Client()
{
	compressed = false;
	encrypted = false;
	state = Handshake;
}

void Client::disconnect(int e)
{
	if (!_playerName.empty())
		syslog(LOG_INFO, "Player %s disconnected: %s\n",
				_playerName.c_str(), strerror(e));
}

void Client::packet(const pkt_t *v)
{
	Packet p(v);
	if (p.err())
		goto drop;

	switch (state) {
	case Handshake:
		handshake(&p);
		break;
	case Status:
		status(&p);
		break;
	case Login:
		login(&p);
		break;
	default:
		goto drop;
	}
	return;
drop:
	p.dump();
}

void Client::handshake(const Packet *p)
{
	switch (p->id()) {
	case 0x00: {	// Handshake
		PktHandshake phs(*p);
		if (phs.err())
			goto drop;
		_proto = phs.protocol();
		switch (phs.next()) {
		case 1:
			state = Status;
			break;
		case 2:
			state = Login;
			tokengen();
			break;
		default:
			goto drop;
		}
		break;
	}
	default:
		goto drop;
	}
	return;

drop:
	p->dump();
}

void Client::status(const Packet *p)
{
	pkt_t pkt;
	switch (p->id()) {
	case 0x00:	// Status request
		pktPushVarInt(&pkt, 0x00);		// ID = 0x00
		pktPushString(&pkt, ::status.toJson());	// JSON response
		break;
	case 0x01: {	// Ping
		PktPing pp(*p);
		if (pp.err())
			goto drop;
		pktPushVarInt(&pkt, 0x01);		// ID = 0x01
		pktPushLong(&pkt, pp.payload());	// Ping Pong!
		break;
	}
	default:
		goto drop;
	}
	hdr->sendPacket(&pkt);
	return;

drop:
	p->dump();
}

void Client::login(const Packet *p)
{
	pkt_t pkt;
	switch (p->id()) {
	case 0x00: {	// Login start
		PktLoginStart pls(*p);
		if (pls.err())
			goto drop;
		_playerName = pls.playerName();
		syslog(LOG_INFO, "Player %s logging in...\n",
				_playerName.c_str());

		// Disconnect
		//pktPushVarInt(&pkt, 0x00);		// ID = 0x00
		//pktPushString(&pkt, "{\"text\":\"Not yet implemented\"}");

		// Encryption request
		pktPushVarInt(&pkt, 0x01);		// ID = 0x01
		pktPushString(&pkt, "");		// Server ID
		BIO *bio = BIO_new(BIO_s_mem());
		::status.pubkey(bio);
		char *p;
		int32_t len = BIO_get_mem_data(bio, &p);
		pktPushVarInt(&pkt, len);		// Public key length
		pktPushByteArray(&pkt, p, len);		// Public key
		BIO_free_all(bio);
		pktPushVarInt(&pkt, _token.size());	// Token length
		pktPushByteArray(&pkt, _token.data(), _token.size());
		hdr->sendPacket(&pkt);
		break;
	}
	case 0x01: {	// Encryption response
		PktLoginResponse plr(*p);
		if (plr.err())
			goto disconnect;
		if (::status.decrypt(plr.secret(), _secret) == -1)
			goto disconnect;
		std::vector<uint8_t> token;
		if (::status.decrypt(plr.token(), token) == -1)
			goto disconnect;
		if (_token.size() != token.size())
			goto disconnect;
		if (strncmp((char *)_token.data(), (char *)token.data(),
					_token.size()) != 0)
			goto disconnect;
		if (!initEncryption())
			goto disconnect;

		// TODO: Enable compression

		// Login success
		pktPushVarInt(&pkt, 0x02);		// ID = 0x02
		pktPushString(&pkt, "11111111-2222-3333-4444-555555555555");
		pktPushString(&pkt, _playerName);
		state = Play;
		hdr->sendPacket(&pkt);

		// Join game
		pkt.clear();
		pktPushVarInt(&pkt, 0x23);		// ID = 0x23
		pktPushInt(&pkt, 123);			// Entity ID
		pktPushUByte(&pkt, 3);			// Gamemode spectator
		pktPushInt(&pkt, 0);			// Dimension overworld
		pktPushUByte(&pkt, 0);			// Difficulty peaceful
		pktPushUByte(&pkt, 0);			// Ignored
		pktPushString(&pkt, "default");		// Level type
		pktPushBoolean(&pkt, false);		// Reduced debug info
		hdr->sendPacket(&pkt);
		break;
	}
	default:
		goto drop;
	}
	return;

disconnect:
	hdr->disconnect();
drop:
	p->dump();
}

void Client::tokengen()
{
	int i = 4;
	while (i--)
		_token.push_back(rand());
}

bool Client::initEncryption()
{
	const auto &evp = EVP_aes_128_cfb8();
	if (EVP_CIPHER_key_length(evp) != (int)_secret.size())
		return false;
	if (EVP_CIPHER_iv_length(evp) != (int)_secret.size())
		return false;
	EVP_CIPHER_CTX_init(&enc);
	EVP_EncryptInit_ex(&enc, evp, 0, _secret.data(), _secret.data());
	EVP_CIPHER_CTX_init(&dec);
	EVP_DecryptInit_ex(&dec, evp, 0, _secret.data(), _secret.data());
	encrypted = true;
	return true;
}

void Client::encrypt(pkt_t *pkt)
{
	int len = 0;
	EVP_EncryptUpdate(&enc, pkt->data(), &len, pkt->data(), pkt->size());
}

void Client::decrypt(pkt_t *pkt)
{
	int len = 0;
	EVP_DecryptUpdate(&dec, pkt->data(), &len, pkt->data(), pkt->size());
}

uint8_t Client::decrypt(uint8_t c)
{
	int len = 0;
	EVP_DecryptUpdate(&dec, &c, &len, &c, 1);
	return c;
}
