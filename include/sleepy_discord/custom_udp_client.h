#pragma once
#include<memory>
#include "udp.h"

namespace SleepyDiscord {
	typedef GenericUDPClient* (*const CustomInitUDPClient)();

	class CustomUDPClient : public GenericUDPClient {
	protected:
		static CustomInitUDPClient init;
		CustomUDPClient() : client(init()) {}
		inline bool connect(const std::string& to, const uint16_t port) override {
			return client->connect(to, port);
		}
		inline bool    send(const uint8_t* buffer, size_t bufferLength) override {
			return client->send(buffer, bufferLength);
		}
		inline std::vector<uint8_t> receive(const std::string& from, const uint16_t port) override {
			return client->receive(from, port);
		}
	private:
		std::unique_ptr<GenericUDPClient> client;
	};

	typedef CustomUDPClient UDPClient;
}