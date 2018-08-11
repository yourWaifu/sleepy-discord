#pragma once
#include <string>
#include "websocket_connection.h"

namespace SleepyDiscord {
	class GenericMessageReceiver {
	public:
		virtual ~GenericMessageReceiver() = default;
		virtual void initialize() {}                                  //called when ready to recevie messages
		virtual void handleFailToConnect() {}                         //called when connection has failed to start
		virtual void processMessage(const std::string & message) = 0; //called when recevicing a message
		virtual void processCloseCode(const int16_t code) {}
		WebsocketConnection connection;                               //maybe this should be set to protected?
	};
}