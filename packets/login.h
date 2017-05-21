#ifndef PKT_LOGIN_H
#define PKT_LOGIN_H

#include <string>
#include <vector>
#include <sstream>
#include "../packet.h"

class PktLoginStart : public Packet
{
public:
	PktLoginStart(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		_playerName = readString(16);
		if (err())
			goto error;
		return;
	error:
		_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << "Login, player " << _playerName;
		Packet::dump(ss.str());
	}

	const std::string& playerName() const {return _playerName;}

private:
	std::string _playerName;
};

class PktLoginResponse : public Packet
{
public:
	PktLoginResponse(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		int32_t len = readVarInt();
		if (err())
			goto error;
		if (!readByteArray(&_secret, len))
			goto error;
		len = readVarInt();
		if (err())
			goto error;
		if (!readByteArray(&_token, len))
			goto error;
		return;
	error:
		_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << "Encryption response, secret(" << _secret.size()
			<< "), token(" << _token.size() << ')';
		Packet::dump(ss.str());
	}

	std::vector<uint8_t> &secret() {return _secret;}
	std::vector<uint8_t> &token() {return _token;}

private:
	std::vector<uint8_t> _secret, _token;
};

#endif
