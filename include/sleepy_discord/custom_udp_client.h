#pragma once
#include<memory>
#include "udp.h"

namespace SleepyDiscord {
	typedef GenericUDPClient* (*const CustomInit);

	class CustomUDPClient : public GenericUDPClient {
	public:

	private:
		std::unique_ptr<GenericUDPClient> client;
	};

	typedef CustomUDPClient UDPClient;
}