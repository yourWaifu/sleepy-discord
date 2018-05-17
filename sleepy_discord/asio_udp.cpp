#include "asio_udp.h"
#ifndef NONEXISTENT_ASIO

namespace SleepyDiscord {
	ASIOUDPClient::ASIOUDPClient() :
		uDPSocket(iOService, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
		resolver (iOService)
	{

	}

	bool ASIOUDPClient::connect(const std::string & to, const uint16_t port) {
		dest = *resolver.resolve({ asio::ip::udp::v4(), to, std::to_string(port) });
		return true;
	}

	void handle_send(
		const std::error_code& /*error*/,
		std::size_t /*bytes_transferred*/) {
	}

	bool ASIOUDPClient::send(const uint8_t * buffer, size_t bufferLength) {
		uDPSocket.async_send_to(asio::buffer(buffer, bufferLength), dest,
			std::bind(&handle_send, std::placeholders::_1, std::placeholders::_2)
		);
		return true;
	}

	std::vector<uint8_t> ASIOUDPClient::receive(const std::string & from, const uint16_t port) {
		//to do maybe remove magic number 256
		uint8_t buffer[256];
		asio::ip::udp::endpoint sender(asio::ip::udp::v4(), port);
		std::size_t btyesReceived = uDPSocket.receive_from(asio::buffer(buffer, 256), sender, 0);
		return std::vector<uint8_t>(buffer, buffer + btyesReceived);
	}
};

#endif