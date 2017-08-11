#pragma once

#include <string>
#include <rapidjson/document.h>

namespace Chat {

class Message
{
public:
	std::string toJson() const;
	std::string toText() const;
	virtual void addTo(rapidjson::Value &v, rapidjson::Document::AllocatorType &a) const {}
	virtual void write(std::string &s) const {}

protected:
private:
};

class Text : public Message
{
public:
	Text() {}
	Text(std::string s) : _text(s) {}
	void addTo(rapidjson::Value &v, rapidjson::Document::AllocatorType &a) const;
	void write(std::string &s) const;

	const std::string &text() const {return _text;}
	void text(std::string s) {_text = s;}

private:
	std::string _text;
};

}
