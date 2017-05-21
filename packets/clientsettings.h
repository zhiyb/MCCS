#ifndef PKT_CLIENTSETTINGS_H
#define PKT_CLIENTSETTINGS_H

class PktClientSettings : public Packet
{
public:
	PktClientSettings(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		PKT_ASSERT(_locale = readString(7));
		PKT_ASSERT(_distance = readByte());
		PKT_ASSERT(_mode = readVarInt());
		PKT_ASSERT(_colors = readBoolean());
		PKT_ASSERT(_skinParts = readUByte());
		PKT_ASSERT(_mainHand = readVarInt());
		return;
	error:	_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << typeid(*this).name();
		PKT_DUMP("locale", _locale);
		PKT_DUMP("distance", (int32_t)_distance);
		PKT_DUMP("mode", _mode);
		PKT_DUMP("colors", _colors);
		PKT_DUMP("skinParts", (uint32_t)_skinParts);
		PKT_DUMP("mainHand", _mainHand);
		Packet::dump(ss.str());
	}

	const std::string &locale() const {return _locale;}
	int8_t distance() const {return _distance;}
	int32_t mode() const {return _mode;}
	bool colors() const {return _colors;}
	uint8_t skinParts() const {return _skinParts;}
	int32_t mainHand() const {return _mainHand;}

private:
	std::string _locale;
	int8_t _distance;
	int32_t _mode;
	bool _colors;
	uint8_t _skinParts;
	int32_t _mainHand;
};

#endif
