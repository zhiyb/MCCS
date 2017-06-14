#ifndef PKT_ENTITYACT_H
#define PKT_ENTITYACT_H

class PktEntityAct : public Packet
{
public:
	PktEntityAct(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		PKT_ASSERT(_eid = readVarInt());
		PKT_ASSERT(_aid = readVarInt());
		PKT_ASSERT(_jmpbst = readVarInt());
		return;
	error:	_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << typeid(*this).name();
		PKT_DUMP("entity id", _eid);
		PKT_DUMP("action id", _aid);
		PKT_DUMP("jump boost", _jmpbst);
		Packet::dump(ss.str());
	}

	int32_t eid() const {return _eid;}
	int32_t aid() const {return _aid;}
	int32_t jumpBoost() const {return _jmpbst;}

private:
	int32_t _eid, _aid, _jmpbst;
};

#endif
