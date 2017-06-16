#pragma once

#include <string>
#include <sstream>
#include <typeinfo>
#include "../packet.h"

#define PKT_ASSERT(v)	v; if (err()) goto error;
#define PKT_DUMP(n, v)	ss << ", " n " " << (v);

#include "tpconfirm.h"
#include "clientsettings.h"
#include "closewindow.h"
#include "pluginmsg.h"
#include "keepalive.h"
#include "playerposition.h"
#include "playerpositionlook.h"
#include "playerlook.h"
#include "entityact.h"

#undef PKT_ASSERT
#undef PKT_DUMP
