#ifndef STATUS_H
#define STATUS_H

#include <string>

class Status
{
public:
	Status() : _online(0) {}

	std::string version() const;
	int protocol() const;
	int playersMax() const;
	int playersOnline() const {return _online;}
	std::string description() const;
	std::string toJson() const;

private:
	int _online;
};

extern Status status;

#endif // STATUS_H
