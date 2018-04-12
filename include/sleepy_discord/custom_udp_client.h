#pragma once
#include<memory>
#include "udp.h"

namespace SleepyDiscord {
	typedef GenericUDPClient* (*const CustomInitUDPClient)();

	class CustomUDPClient : public GenericUDPClient {
	public:
		static CustomInitUDPClient init;
		CustomUDPClient() : client(init()) {}
	private:
		std::unique_ptr<GenericUDPClient> client;
	};

	typedef CustomUDPClient UDPClient;
}