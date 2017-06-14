#ifndef PKT_CLOSEWINDOW_H
#define PKT_CLOSEWINDOW_H

class PktCloseWindow : public Packet
{
public:
	PktCloseWindow(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		PKT_ASSERT(_wid = readUByte());
		return;
	error:	_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << typeid(*this).name();
		PKT_DUMP("window id", (unsigned int)_wid);
		Packet::dump(ss.str());
	}

	uint8_t wid() const {return _wid;}

private:
	uint8_t _wid;
};

#endif
