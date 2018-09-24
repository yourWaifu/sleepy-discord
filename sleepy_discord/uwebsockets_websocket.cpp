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
			auto connection = static_cast<uWS::WebSocket<uWS::CLIENT>**>(ws->getUserData());
			*connection = ws;
		});
		hub.onMessage([=](uWS::WebSocket<uWS::CLIENT>* ws, char * message, size_t length, uWS::OpCode opCode) {
			processMessage(message);
		});
		hub.onError([=](void *user) {
			isConnectionBad = true;
		});

		start(token, numOfThreads);
	}

	bool UWebSocketsDiscordClient::connect(const std::string & uri, GenericMessageReceiver* messageProcessor, WebsocketConnection* connection) {
		isConnectionBad = false;
		hub.connect(uri, connection);
		return !isConnectionBad;
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
		return Timer(
			[timer]() {
				timer->stop();
				timer->close();
			}
		);
	}

	void UWebSocketsDiscordClient::runAsync() {
		thread = std::thread([this]() { hub.run(); });
	}

	void UWebSocketsDiscordClient::disconnect(unsigned int code, const std::string reason, WebsocketConnection* connection) {
		/*the static cast should do a compile time check that WebsocketConnection
		  is the correct type*/
		static_cast<uWS::WebSocket<uWS::CLIENT>*>(*connection)->close();
	}

	void UWebSocketsDiscordClient::send(std::string message, WebsocketConnection* connection) {
		//if disconnect doesn't give an error then this should also be fine
		(*connection)->send(message.c_str());
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