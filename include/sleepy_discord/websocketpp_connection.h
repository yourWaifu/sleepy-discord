#pragma once
#include "websocketpp_common.h"

namespace SleepyDiscord {
#ifndef NONEXISTENT_WEBSOCKETPP
	typedef websocketpp::connection_hdl WebsocketConnection;
#endif
}