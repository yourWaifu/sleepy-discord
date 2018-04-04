#pragma once
#include "asio_include.h"
#ifndef NONEXISTENT_ASIO
#include "udp.h"

namespace SleepyDiscord {
	class ASIOUDPClient : public GenericUDPClient {
	public:
		ASIOUDPClient();
	protected:
		bool                 send   (const std::string& to  , const uint16_t port, const uint8_t* buffer, size_t bufferLength);
		std::vector<uint8_t> receive(const std::string& from, const uint16_t port                                            );
	private:
		asio::io_service iOService;
		asio::ip::udp::socket uDPSocket;
		asio::ip::udp::resolver resolver;
	};

	typedef ASIOUDPClient UDPClient;
};
#endif