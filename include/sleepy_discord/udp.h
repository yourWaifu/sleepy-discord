#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <functional>

namespace SleepyDiscord {
	class GenericUDPClient {
	public:
		typedef std::function<void()> SendHandler;
		typedef std::function<void(const std::vector<uint8_t>&)> ReceiveHandler;

		virtual bool connect(const std::string& to, const uint16_t port) = 0;
		virtual void send(
			const uint8_t* buffer,
			size_t bufferLength,
			SendHandler handler = [](){}
		) = 0;
		virtual void receive(ReceiveHandler handler) = 0;

		inline void send(const std::vector<uint8_t> buffer, SendHandler handler = [](){}) {
			send(&buffer[0], buffer.size(), handler);
		}
	};
}