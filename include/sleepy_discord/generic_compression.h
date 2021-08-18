#pragma once
#include <string>

namespace SleepyDiscord {
	class GenericCompression {
	public:
		virtual ~GenericCompression() = default;
		virtual void uncompress(const std::string& compressed) = 0;
		virtual void getOutput(std::string& uncompressedOut) = 0;
		virtual void resetStream() = 0;
		virtual bool streamEnded() = 0;
	};
}