#pragma once
#include "asio_include.h"
#ifndef NONEXISTENT_ASIO
#include "udp.h"

namespace SleepyDiscord {
	class ASIOUDPClient : public GenericUDPClient {
	public:
		ASIOUDPClient();
	protected:
		virtual bool         connect(const std::string& to  , const uint16_t port                                            ) override;
		virtual bool         send   (                                              const uint8_t* buffer, size_t bufferLength) override;
		std::vector<uint8_t> receive(const std::string& from, const uint16_t port                                            ) override;
	private:
		asio::io_service iOService;
		asio::ip::udp::socket uDPSocket;
		asio::ip::udp::resolver resolver;
		asio::ip::udp::endpoint dest;
	};

	typedef ASIOUDPClient UDPClient;
};
#endif