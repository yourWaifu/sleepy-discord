#pragma once
#include <uWS/uWS.h>
#include <thread>
#include "client.h"
#ifndef NONEXISTENT_UWEBSOCKETS

namespace SleepyDiscord {

	class UWebSocketsDiscordClient : public BaseDiscordClient {
	public:
		UWebSocketsDiscordClient() : maxNumOfThreads(0) {}
		UWebSocketsDiscordClient(const std::string token, const char numOfThreads = 3);
		~UWebSocketsDiscordClient();
	private:
		uWS::Hub hub;
		bool connect(const std::string & uri);
		void disconnect(unsigned int code, const std::string reason);
		void send(std::string message);
		uWS::WebSocket<uWS::CLIENT> theClient;
		std::thread thread;
		const char maxNumOfThreads;
		bool isConnectionBad;
	};

	typedef UWebSocketsDiscordClient DiscordClient;
}
#endif