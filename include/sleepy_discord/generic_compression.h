#pragma once

namespace SleepyDiscord {
	class GenericCompression {
	public:
		virtual bool uncompress(const std::string& compressed, std::string& uncompressedOut) = 0;
	};
}