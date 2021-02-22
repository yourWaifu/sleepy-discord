#pragma once
#include "generic_compression.h"
#include "zlib-ng/zlib-ng.h"
#include <array>
#include <list>
#include <string>
#include <mutex>

namespace SleepyDiscord {
	class ZLibCompression : public GenericCompression {
	public:
		ZLibCompression();

		~ZLibCompression() {
			zng_inflateEnd(&stream);
		}

		zng_stream stream;
		int statusCode;

		constexpr static size_t chunkSize = 16 * 1024;
		using Data = std::array<char, chunkSize>;
		using Buffer = std::pair<Data, std::size_t>;
		std::list<Buffer> outputQueue;
		std::mutex mutex; //only allow one thread to uncompress

		void uncompress(const std::string& compressed) override;
		void getOutput(std::string& uncompressedOut) override;

		void resetStream() override {
			zng_inflateReset(&stream);
		}

		bool streamEnded() override {
			return statusCode == Z_STREAM_END;
		}
	};

	using DefaultCompression = ZLibCompression;
#define SLEEPY_DEFAULT_COMPRESSION ZLibCompression
}