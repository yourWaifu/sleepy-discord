#pragma once
#include <memory>
#include "websocket.h"

namespace SleepyDiscord {
	typedef GenericWebsocketConnection* (*const CustomInitWebsocketConnection)();

	struct CustomWebsocketConnection : public GenericWebsocketConnection {
		static CustomInitWebsocketConnection init;
		CustomWebsocketConnection() : connection(init()) {}
		inline GenericWebsocketConnection* get() {
			return connection.get();
		}
	private:
		std::unique_ptr<GenericWebsocketConnection> connection;
	};

	typedef CustomWebsocketConnection WebsocketConnection;
}