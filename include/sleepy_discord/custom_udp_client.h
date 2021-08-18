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
		inline void send(
			const uint8_t* buffer,
			size_t bufferLength,
			SendHandler handler = []() {}
		) override {
			return client->send(buffer, bufferLength, handler);
		}
		inline void receive(ReceiveHandler handler) override {
			return client->receive(handler);
		}
	private:
		std::unique_ptr<GenericUDPClient> client;
	};

	typedef CustomUDPClient UDPClient;
}