#pragma once
#include <memory>
#include "websocket.h"

namespace SleepyDiscord {
	typedef GenericWebsocketsClient* (*const CustomInit);

	class CustomWebsocketClient : public GenericWebsocketsClient {
	public:

	private:
		std::unique_ptr<GenericWebsocketsClient> client;
	};

	typedef CustomWebsocketClient WebsocketClient;
}