#include "uwebsockets_websocket.h"
#ifndef NONEXISTENT_UWEBSOCKETS

namespace SleepyDiscord {
	UWebSocketsDiscordClient::~UWebSocketsDiscordClient() {
	}

	UWebSocketsDiscordClient::UWebSocketsDiscordClient(const std::string token) {
		start(token);
	}

	bool UWebSocketsDiscordClient::connect(const std::string & uri) {
		hub.onMessage([](uWS::WebSocket<uWS::SERVER> ws, char *message, size_t length, uWS::OpCode opCode) {
			processMessage(message);
		});

		hub.connect(uri, nullptr);
		hub.run()
		return 0;
	}

	void UWebSocketsDiscordClient::send(std::string message) {

	}

}
#endif