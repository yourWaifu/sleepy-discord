#pragma once
#if !defined(NONEXISTENT_UWEBSOCKETS) | defined(EXISTENT_UWEBSOCKETS)
#include <uWS/uWS.h>
#endif
#ifndef NONEXISTENT_UWEBSOCKETS
#include <thread>
#define SLEEPY_UWEBSOCKETS 563146 //random number that's only used once in the code
#define SLEEPY_LOCK_EXISTENT_TO SLEEPY_UWEBSOCKETS //stops the compiler from defining things from other libraries
#include "client.h"

namespace SleepyDiscord {
	class UWebSocketsDiscordClient : public BaseDiscordClient {
	public:
		UWebSocketsDiscordClient() : maxNumOfThreads(0) {}
		UWebSocketsDiscordClient(const std::string token, const char numOfThreads = 3);
		~UWebSocketsDiscordClient();
		void run();
		Timer schedule(std::function<void()> code, const time_t milliseconds);
	protected:
#include "standard_config_header.h"
	private:
		uWS::Hub hub;
		bool connect(const std::string & uri, GenericMessageReceiver* messageProcessor, WebsocketConnection* connection);
		void disconnect(unsigned int code, const std::string reason, WebsocketConnection* connection);
		void send(std::string message, WebsocketConnection* connection);
		void runAsync();
		std::thread thread;
		const char maxNumOfThreads;
		bool isConnectionBad = false;
		SLEEPY_LOCK_CLIENT_FUNCTIONS
	};

	typedef UWebSocketsDiscordClient DiscordClient;
}
#undef SLEEPY_UWEBSOCKETS
#undef SLEEPY_LOCK_TO_UWEBSOCKETS
#endif