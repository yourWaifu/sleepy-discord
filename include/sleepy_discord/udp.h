#pragma once
#include <string>
#include <cstdint>
#include <vector>

namespace SleepyDiscord {
	class GenericUDPClient {
	public:
	protected:
		virtual bool                 send   (const std::string& to  , const uint16_t port, const uint8_t* buffer, size_t bufferLength) = 0;
		virtual std::vector<uint8_t> receive(const std::string& from, const uint16_t port                                            ) = 0;

		inline bool send(const std::string& to, const uint16_t port, const std::vector<uint8_t> buffer) {
			return send(to, port, &buffer[0], buffer.size());
		}
	};
}