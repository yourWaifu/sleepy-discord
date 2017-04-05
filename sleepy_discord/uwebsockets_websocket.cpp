#include "uwebsockets_websocket.h"
#ifndef NONEXISTENT_UWEBSOCKETS

namespace SleepyDiscord {
	UWebSocketsDiscordClient::~UWebSocketsDiscordClient() {
	}

	UWebSocketsDiscordClient::UWebSocketsDiscordClient(const std::string token) {
		hub.onConnection([this](uWS::WebSocket<uWS::CLIENT> ws, uWS::HttpRequest req) {
			theClient = ws;
		});
		hub.onMessage([this](uWS::WebSocket<uWS::CLIENT> ws, char * message, size_t length, uWS::OpCode opCode) {
			processMessage(message);
		});

		start(token);
	}

	bool UWebSocketsDiscordClient::connect(const std::string & uri) {
		//hub.listen(????);
		hub.connect(uri, nullptr);
		hub.run();
		return 0;
	}

	void UWebSocketsDiscordClient::disconnect(unsigned int code, const std::string reason) {
		theClient.close();
	}

	void UWebSocketsDiscordClient::send(std::string message) {
		theClient.send(message.c_str());
	}



/* list of needed librarys to compile this
uWS.lib
libuv.lib
iphlpapi.lib
psapi.lib
userenv.lib
zlibstat.lib
*/


}
#endif