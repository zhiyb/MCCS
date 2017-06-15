#include <string.h>
#include <iostream>
#include <openssl/bio.h>
#include "logging.h"
#include "client.h"
#include "handler.h"
#include "status.h"
#include "packets/packets.h"
#include "protocols/id.h"

using namespace Protocol;

Client::Client()
{
	_protocol = 0;
	_version = 0;
	compressed = false;
	encrypted = false;
	state = State::Handshake;
}

void Client::logDisconnect(int e)
{
	if (_playerName.empty())
		return;
	logger->info("Player {} disconnected: {}", _playerName.c_str(), strerror(e));
}

void Client::packet(pkt_t *v)
{
	if (v->empty())
		return;
	Packet p(v);
	if (p.err()) {
		p.dump();
		return;
	}
	pktid_t i = id(p.id());
	if (i < 0) {
		p.dump();
		return;
	}
	p.setID(i);
	switch (state) {
	case State::Handshake:
		handshake(&p);
		break;
	case State::Status:
		status(&p);
		break;
	case State::Login:
		login(&p);
		break;
	case State::Play:
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
	case Handshake::Server::Handshake: {
		PktHandshake phs(*p);
		if (phs.err())
			break;
		_version = phs.protocol();
		_protocol = protocols.fuzzyVersion(_version);
		switch (phs.next()) {
		case 1:
			state = State::Status;
			return;
		case 2:
			state = State::Login;
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
	case Status::Server::Request:
		pktPushVarInt(&pkt, pktid(Status::Client::Response));
		pktPushString(&pkt, ::status.toJson());			// JSON response
		hdr->send(&pkt);
		return;
	case Status::Server::Ping: {
		PktPing pp(*p);
		if (pp.err())
			break;
		pktPushVarInt(&pkt, pktid(Status::Client::Pong));
		pktPushLong(&pkt, pp.payload());			// Ping Pong!
		hdr->send(&pkt);
		return;
	}
	}
	p->dump();
}

void Client::login(const Packet *p)
{
	pkt_t pkt;
	switch (p->id()) {
	case Login::Server::Start: {	// Login start
		PktLoginStart pls(*p);
		if (pls.err())
			goto drop;
		_playerName = pls.playerName();

		// Disconnection
		/*if (!protocols.hasVersion(_version)) {
			pktPushVarInt(&pkt, pktid(Login::Client::Disconnect));
			pktPushString(&pkt, "{\"text\":\"Client version not supported\"}");
			hdr->send(&pkt);
			hdr->disconnect();
			break;
		}*/

		// Encryption request
		pktPushVarInt(&pkt, pktid(Login::Client::Encryption));
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
		hdr->send(&pkt);
		break;
	}
	case Login::Server::Encryption: {	// Encryption response
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
		pktPushVarInt(&pkt, pktid(Login::Client::Success));
		pktPushString(&pkt, "11111111-2222-3333-4444-555555555555");
		pktPushString(&pkt, _playerName);
		hdr->send(&pkt);
		state = State::Play;
		logger->info("Player {} logged in", _playerName.c_str());
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

void Client::encryptAppend(pkt_t *src, pkt_t *dst)
{
	size_t s = dst->size();
	dst->resize(s + src->size());
	int len = 0;
	EVP_EncryptUpdate(&enc, dst->data() + s, &len, src->data(), src->size());
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
