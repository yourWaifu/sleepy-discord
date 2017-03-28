#pragma once
#include <uWS/uWS.h>
#include "client.h"
#ifndef NONEXISTENT_UWEBSOCKETS

namespace SleepyDiscord {

	class UWebSocketsDiscordClient : public DiscordClient {
	public:
		UWebSocketsDiscordClient() {}
		UWebSocketsDiscordClient(const std::string token);
		~UWebSocketsDiscordClient();
	private:
		uWS::Hub hub;
		bool connect(const std::string & uri);
		void send(std::string message);
	};

}
#endif