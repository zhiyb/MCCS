#ifndef PROTOCOL_332_CLIENT_H
#define PROTOCOL_332_CLIENT_H

#include "../client.h"

namespace Protocol {
namespace p332 {

class ClientProtocol : public Client
{
protected:
	virtual void play(const Packet *p);
	virtual void playInit();
};

}
}

#endif
