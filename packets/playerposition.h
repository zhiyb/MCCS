#pragma once

class PktPlayerPosition : public Packet
{
public:
	PktPlayerPosition(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		PKT_ASSERT(_x = readDouble());
		PKT_ASSERT(_y = readDouble());
		PKT_ASSERT(_z = readDouble());
		PKT_ASSERT(_onGround = readBoolean());
		return;
	error:	_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << typeid(*this).name();
		PKT_DUMP("x", _x);
		PKT_DUMP("y", _y);
		PKT_DUMP("z", _z);
		PKT_DUMP("onGround", _onGround);
		Packet::dump(ss.str());
	}

	double x() const {return _x;}
	double y() const {return _y;}
	double z() const {return _z;}
	bool onGround() const {return _onGround;}

private:
	double _x, _y, _z;
	bool _onGround;
};
