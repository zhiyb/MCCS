#include <sstream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include "chat.h"

using namespace std;
using namespace rapidjson;
using namespace Chat;

string Message::toJson() const
{
	using namespace rapidjson;
	Document d(kObjectType);
	addTo(d, d.GetAllocator());
	stringstream ss;
	OStreamWrapper osw(ss);
	Writer<OStreamWrapper> writer(osw);
	d.Accept(writer);
	return ss.str();
}

string Message::toText() const
{
	string s;
	write(s);
	return s;
}

void Text::addTo(Value &v, Document::AllocatorType &a) const
{
	v.AddMember("text", StringRef(text().c_str()), a);
}

void Text::write(string &s) const
{
	s.append(_text);
}
