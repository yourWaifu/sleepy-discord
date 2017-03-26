#include "uwebsockets_websocket.h"
#ifndef NONEXISTENT_UWEBSOCKETS

namespace SleepyDiscord {
	UWebSocketsDiscordClient::~UWebSocketsDiscordClient() {
	}

	UWebSocketsDiscordClient::UWebSocketsDiscordClient(const std::string token) {
		start(token);
	}

	int UWebSocketsDiscordClient::connect(const std::string & uri) {
		hub.onMessage([](uWS::WebSocket<uWS::SERVER> ws, char *message, size_t length, uWS::OpCode opCode) {
			processMessage(message);
		});

		hub.connect("wss://localhost:3000", nullptr);
		hub.run()
		return 0;
	}

	void UWebSocketsDiscordClient::send(std::string message) {

	}

}
#endif