#include "asio_udp.h"

namespace SleepyDiscord {
	ASIOUDPClient::ASIOUDPClient() :
		uDPSocket(iOService, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
		resolver (iOService)
	{

	}

	bool ASIOUDPClient::send(const std::string & to, const uint16_t port, const uint8_t* buffer, size_t bufferLength) {
		asio::error_code error;
		const asio::ip::udp::endpoint& dest = *resolver.resolve({ asio::ip::udp::v4(), to, std::to_string(port) });
		uDPSocket.send_to(asio::buffer(buffer, bufferLength), dest, 0, error);
		return !error;
	}

	std::vector<uint8_t> ASIOUDPClient::receive(const std::string & from, const uint16_t port) {
		//to do maybe remove magic number 256
		uint8_t buffer[256];
		return std::vector<uint8_t>(buffer, buffer + uDPSocket.receive_from(asio::buffer(buffer, 256), asio::ip::udp::endpoint(asio::ip::udp::v4(), port)));
	}
};