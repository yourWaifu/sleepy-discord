#include "asio_udp.h"
#ifndef NONEXISTENT_ASIO

namespace SleepyDiscord {
	ASIOUDPClient::ASIOUDPClient() :
		uDPSocket(iOService, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
		resolver (iOService)
	{

	}

	bool ASIOUDPClient::connect(const std::string & to, const uint16_t port) {
		endpoint = *resolver.resolve({ asio::ip::udp::v4(), to, std::to_string(port) });
		return true;
	}

	void handle_send(
		const std::error_code& /*error*/,
		std::size_t /*bytes_transferred*/,
		GenericUDPClient::SendHandler handler
	) {
		handler();
	}

	void ASIOUDPClient::send(
		const uint8_t* buffer,
		size_t bufferLength,
		SendHandler handler
	) {
		uDPSocket.async_send_to(asio::buffer(buffer, bufferLength), endpoint,
			std::bind(&handle_send, std::placeholders::_1, std::placeholders::_2, handler)
		);
	}

	void handle_receive(
		const std::error_code& /*error*/,
		std::size_t /*bytes_transferred*/
	) {

	}

	void ASIOUDPClient::receive(ReceiveHandler handler) {
		constexpr std::size_t bufferSize = 1 << 16;
		uint8_t buffer[bufferSize];
		std::size_t btyesReceived = uDPSocket.receive_from(asio::buffer(buffer, bufferSize), endpoint, 0);
		handler(std::vector<uint8_t>(buffer, buffer + btyesReceived));
	}
};

#endif