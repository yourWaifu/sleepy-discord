#pragma once
#include <string>
#include <cstdint>
#include <vector>

namespace SleepyDiscord {
	class GenericUDPClient {
	public:
		friend class CustomUDPClient;	//I don't know if this'll work but hopefuly it does
	protected:
		virtual bool                 connect(const std::string& to  , const uint16_t port                                            ) = 0;
		virtual bool                 send   (                                              const uint8_t* buffer, size_t bufferLength) = 0;
		virtual std::vector<uint8_t> receive(const std::string& from, const uint16_t port                                            ) = 0;

		inline bool send(const std::vector<uint8_t> buffer) {
			return send(&buffer[0], buffer.size());
		}
	};
}