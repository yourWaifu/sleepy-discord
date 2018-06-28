#pragma once
#include "asio_include.h"
#ifndef NONEXISTENT_ASIO
#include "udp.h"

namespace SleepyDiscord {
	class ASIOUDPClient : public GenericUDPClient {
	public:
		ASIOUDPClient();
		bool connect(const std::string& to  , const uint16_t port) override;
		void send(
			const uint8_t* buffer,
			size_t bufferLength,
			SendHandler handler = []() {}
		) override;
		void receive(ReceiveHandler handler) override;
	private:
		asio::io_service iOService;
		asio::ip::udp::socket uDPSocket;
		asio::ip::udp::resolver resolver;
		asio::ip::udp::endpoint endpoint;
	};

	typedef ASIOUDPClient UDPClient;
};
#endif