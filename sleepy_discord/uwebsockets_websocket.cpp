#include "uwebsockets_websocket.h"
#ifndef NONEXISTENT_UWEBSOCKETS

namespace SleepyDiscord {
	UWebSocketsDiscordClient::~UWebSocketsDiscordClient() {
		thread.join();
	}

	UWebSocketsDiscordClient::UWebSocketsDiscordClient(const std::string token, const char numOfThreads) :
		maxNumOfThreads(numOfThreads)
	{
		hub.onConnection([this](uWS::WebSocket<uWS::CLIENT> ws, uWS::HttpRequest req) {
			theClient = ws;
		});
		hub.onMessage([this](uWS::WebSocket<uWS::CLIENT> ws, char * message, size_t length, uWS::OpCode opCode) {
			processMessage(message);
		});
		hub.onError([this](void *user) {
			isConnectionBad = true;
		});

		start(token, numOfThreads);
	}

	bool UWebSocketsDiscordClient::connect(const std::string & uri) {
		isConnectionBad = false;
		hub.connect(uri, nullptr);
		if (isConnectionBad) return false;
		if (2 < maxNumOfThreads) thread = std::thread([this]() { hub.run(); });
		else hub.run();
		return true;
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