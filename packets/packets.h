#ifndef PKT_PACKETS_H
#define PKT_PACKETS_H

#include <string>
#include <sstream>
#include <typeinfo>
#include "../packet.h"

#define PKT_ASSERT(v)	v; if (err()) goto error;
#define PKT_DUMP(n, v)	ss << ", " n " " << (v);

#include "handshake.h"
#include "ping.h"
#include "login.h"

#include "tpconfirm.h"
#include "clientsettings.h"
#include "pluginmsg.h"
#include "keepalive.h"
#include "playerpos.h"
#include "playerposlook.h"
#include "playerlook.h"

#undef PKT_ASSERT

#endif // PKT_PACKETS_H
