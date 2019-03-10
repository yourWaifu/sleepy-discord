#pragma once
#include <memory>
#include "websocket.h"

namespace SleepyDiscord {
	typedef std::shared_ptr<GenericWebsocketConnection> WebsocketConnection;
}