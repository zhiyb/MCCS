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

private:
	int _online;
	RSA *rsa;
};

extern Status status;

#endif // STATUS_H
