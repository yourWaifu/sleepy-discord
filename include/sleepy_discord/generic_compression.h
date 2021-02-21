#pragma once

namespace SleepyDiscord {
	class GenericCompression {
	public:
		virtual ~GenericCompression() = 0;
		virtual bool uncompress(const std::string& compressed, std::string& uncompressedOut) = 0;
	};
}