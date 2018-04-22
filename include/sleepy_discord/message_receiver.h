#pragma once
#include <string>
#include "websocket_connection.h"

namespace SleepyDiscord {
	class GenericMessageReceiver {
	public:
		virtual void initialize() {}                          //called when ready to recevie messages
		virtual void processMessage(std::string message) = 0; //called when recevicing a message
		WebsocketConnection connection;                       //maybe this should be set to protected?
	};
}