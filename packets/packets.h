#pragma once

#include <string>
#include <sstream>
#include <typeinfo>
#include "../packet.h"

#define PKT_ASSERT(v)	v; if (err()) goto error;
#define PKT_DUMP(n, v)	ss << ", " n " " << (v);

#include "handshake.h"
#include "ping.h"
#include "login.h"

#undef PKT_ASSERT
#undef PKT_DUMP
