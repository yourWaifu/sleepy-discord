#include "uwebsockets_websocket.h"
#ifndef NONEXISTENT_UWEBSOCKETS
#include <iostream>

namespace SleepyDiscord {
	UWebSocketsDiscordClient::~UWebSocketsDiscordClient() {
		thread.join();
	}

	UWebSocketsDiscordClient::UWebSocketsDiscordClient(const std::string token, const char numOfThreads) :
		maxNumOfThreads(numOfThreads) {
		hub.onConnection([=](uWS::WebSocket<uWS::CLIENT>* ws, uWS::HttpRequest req) {
			theClient = ws;
		});
		hub.onMessage([=](uWS::WebSocket<uWS::CLIENT>* ws, char * message, size_t length, uWS::OpCode opCode) {
			theClient = ws;
			processMessage(message);
		});
		hub.onError([=](void *user) {
			isConnectionBad = true;
		});

		start(token, numOfThreads);
	}

	bool UWebSocketsDiscordClient::connect(const std::string & uri) {
		isConnectionBad = false;
		hub.connect(uri, nullptr);
		if (isConnectionBad) return false;
		return true;
	}

	void UWebSocketsDiscordClient::run() {
		hub.run();
	}

	Timer UWebSocketsDiscordClient::schedule(std::function<void()> code, const time_t milliseconds) {
		uS::Timer *timer = new uS::Timer(hub.getLoop());
		timer->setData(&code);
		timer->start([](uS::Timer *timer) {
			(*static_cast<std::function<void()>*>(timer->getData()))();
			timer->close();
		}, milliseconds, 0);
		return [timer]() {
			timer->stop();
			timer->close();
		};
	}

	void UWebSocketsDiscordClient::runAsync() {
		thread = std::thread([this]() { hub.run(); });
	}

	void UWebSocketsDiscordClient::disconnect(unsigned int code, const std::string reason) {
		theClient->close();
	}

	void UWebSocketsDiscordClient::send(std::string message) {
		theClient->send(message.c_str());
	}

#include "standard_config.h"


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