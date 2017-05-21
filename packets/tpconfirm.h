#ifndef PKT_TPCONFIRM_H
#define PKT_TPCONFIRM_H

class PktTPConfirm : public Packet
{
public:
	PktTPConfirm(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		PKT_ASSERT(_id = readVarInt());
		return;
	error:	_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << typeid(*this).name();
		PKT_DUMP("id", _id);
		Packet::dump(ss.str());
	}

	int32_t id() const {return _id;}

private:
	int32_t _id;
};

#endif
