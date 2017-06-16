#pragma once

#include <string>
#include <rapidjson/document.h>

namespace Chat {

class Message
{
public:
	std::string toJson() const;
	std::string toText() const;
	virtual void write(rapidjson::Value &v, rapidjson::Document::AllocatorType &a) const = 0;
	virtual void write(std::string &s) const {}

protected:
private:
};

class Text : public Message
{
public:
	Text() {}
	Text(std::string s) : _text(s) {}
	void write(rapidjson::Value &v, rapidjson::Document::AllocatorType &a) const;
	void write(std::string &s) const;

	std::string text() const {return _text;}
	void text(std::string s) {_text = s;}

private:
	std::string _text;
};

}
