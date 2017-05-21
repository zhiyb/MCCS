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
	if (p.err()) {
		p.dump();
		return;
	}
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
	case Play:
		play(&p);
		break;
	default:
		p.dump();
		hdr->disconnect();
	}
}

void Client::handshake(const Packet *p)
{
	switch (p->id()) {
	case 0x00: {	// Handshake
		PktHandshake phs(*p);
		if (phs.err())
			break;
		_proto = phs.protocol();
		switch (phs.next()) {
		case 1:
			state = Status;
			return;
		case 2:
			state = Login;
			tokengen();
			return;
		}
	}
	}
	p->dump();
	hdr->disconnect();
}

void Client::status(const Packet *p)
{
	pkt_t pkt;
	switch (p->id()) {
	case 0x00:	// Status request
		pktPushVarInt(&pkt, 0x00);		// ID = 0x00
		pktPushString(&pkt, ::status.toJson());	// JSON response
		hdr->sendPacket(&pkt);
		return;
	case 0x01: {	// Ping
		PktPing pp(*p);
		if (pp.err())
			break;
		pktPushVarInt(&pkt, 0x01);		// ID = 0x01
		pktPushLong(&pkt, pp.payload());	// Ping Pong!
		hdr->sendPacket(&pkt);
		return;
	}
	}
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
		// TODO: On-line authentication

		// Login success
		pktPushVarInt(&pkt, 0x02);		// ID = 0x02
		pktPushString(&pkt, "11111111-2222-3333-4444-555555555555");
		pktPushString(&pkt, _playerName);
		state = Play;
		hdr->sendPacket(&pkt);
		syslog(LOG_INFO, "Player %s logged in\n", _playerName.c_str());
		playInit();
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
