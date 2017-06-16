#pragma once

class PktPlayerLook : public Packet
{
public:
	PktPlayerLook(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		PKT_ASSERT(_yaw = readFloat());
		PKT_ASSERT(_pitch = readFloat());
		PKT_ASSERT(_onGround = readBoolean());
		return;
	error:	_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << typeid(*this).name();
		PKT_DUMP("yaw", _yaw);
		PKT_DUMP("pitch", _pitch);
		PKT_DUMP("onGround", _onGround);
		Packet::dump(ss.str());
	}

	float yaw() const {return _yaw;}
	float pitch() const {return _pitch;}
	bool onGround() const {return _onGround;}

private:
	float _yaw, _pitch;
	bool _onGround;
};
