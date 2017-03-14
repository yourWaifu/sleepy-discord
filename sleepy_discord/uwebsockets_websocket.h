#pragma once
#include "client.h"
//#include <uWS/uWS.h>

namespace SleepyDiscord {

	class UWebSocketsDiscordClient : public DiscordClient {
	public:
		UWebSocketsDiscordClient() {}
		//UWebSocketsDiscordClient(const std::string token);
		~UWebSocketsDiscordClient();
	private:
		//uWS::Hub hub;
		int connect(const std::string & uri);
		void send(std::string message);
	};

}
