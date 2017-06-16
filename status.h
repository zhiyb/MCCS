#ifndef STATUS_H
#define STATUS_H

#include <stdint.h>
#include <string>
#include <vector>
#include <openssl/rsa.h>
#include <openssl/bio.h>

class Status
{
public:
	Status();
	~Status();

	int keygen();
	int pubkey(BIO *bio);
	int decrypt(std::vector<uint8_t> &from, std::vector<uint8_t> &to);

	std::string version() const;
	int protocol() const;
	int playersMax() const;
	int playersOnline() const {return _online;}
	std::string description() const;
	std::string toJson() const;

	uint8_t difficulty() const {return _difficulty;}
	void difficulty(uint8_t v) {_difficulty = v;}
	const std::string &levelType() const {return _level;}
	void levelType(const std::string &s) {_level = s;}
	bool debug() const {return _debug;}
	void debug(bool e) {_debug = e;}

private:
	int _online;
	uint8_t _difficulty;	// enum: Difficulty
	std::string _level;	// Level type
	bool _debug;		// Client debug info

	RSA *rsa;
};

extern Status status;

#endif // STATUS_H
