#ifndef PKT_PLUGINMSG_H
#define PKT_PLUGINMSG_H

class PktPluginMsg : public Packet
{
public:
	PktPluginMsg(const Packet &p) : Packet(p)
	{
		if (!valid())
			return;
		PKT_ASSERT(_channel = readString(20));
		PKT_ASSERT(readByteArray(&_data, length()));
		return;
	error:	_valid = false;
	}

	virtual void dump() const
	{
		std::stringstream ss;
		ss << typeid(*this).name();
		PKT_DUMP("channel", _channel);
		PKT_DUMP("data#", _data.size());
		Packet::dump(ss.str());
	}

	const std::string &channel() const {return _channel;}
	const byteArray_t &data() const {return _data;}

private:
	std::string _channel;
	byteArray_t _data;
};

#endif
