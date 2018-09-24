#include "asio_udp.h"
#ifndef NONEXISTENT_ASIO

namespace SleepyDiscord {
	ASIOUDPClient::ASIOUDPClient() : ASIOUDPClient(asio::io_service()) {}

	ASIOUDPClient::ASIOUDPClient(asio::io_service& service) :
		iOService(&service),
		uDPSocket(*iOService, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
		resolver (*iOService)
	{

	}

	bool ASIOUDPClient::connect(const std::string & to, const uint16_t port) {
		if (iOService == nullptr) return false;
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
		if (iOService == nullptr) return;
		uDPSocket.async_send_to(asio::buffer(buffer, bufferLength), endpoint,
			std::bind(&handle_send, std::placeholders::_1, std::placeholders::_2, handler)
		);
	}

	void ASIOUDPClient::receive(ReceiveHandler handler) {
		if (iOService == nullptr) return;
		uDPSocket.async_receive_from(asio::buffer(buffer, bufferSize), endpoint, 0,
			std::bind(
				&ASIOUDPClient::handle_receive, this, std::placeholders::_1,
				std::placeholders::_2, handler
			)
		);
	}

	void ASIOUDPClient::handle_receive(
		const std::error_code& /*error*/,
		std::size_t bytes_transferred,
		ReceiveHandler handler
	) {
		handler(std::vector<uint8_t>(buffer, buffer + bytes_transferred));
	}
};

#endif