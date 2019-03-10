#pragma once
#include "asio_include.h"
#ifndef NONEXISTENT_ASIO
#include "udp.h"

namespace SleepyDiscord {

	class BaseDiscordClient;

	class ASIOUDPClient : public GenericUDPClient {
	public:
		//ASIOUDPClient();
		ASIOUDPClient(BaseDiscordClient& client);
		ASIOUDPClient(asio::io_service& service);
		bool connect(const std::string& to  , const uint16_t port) override;
		void send(
			const uint8_t* buffer,
			size_t bufferLength,
			SendHandler handler = []() {}
		) override;
		void receive(ReceiveHandler handler) override;
	private:
		asio::io_service* iOService;
		asio::ip::udp::socket uDPSocket;
		asio::ip::udp::resolver resolver;
		asio::ip::udp::endpoint endpoint;

		void handle_receive(const std::error_code &, std::size_t bytes_transferred, ReceiveHandler handler);

		constexpr static std::size_t bufferSize = 1 << 16;
		uint8_t buffer[bufferSize];
	};

	typedef ASIOUDPClient UDPClient;
};
#endif