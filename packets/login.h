#ifndef PKT_LOGIN_H
#define PKT_LOGIN_H

#include <string>
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

#endif
