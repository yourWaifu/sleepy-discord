#pragma once
#include "generic_compression.h"
#include "zlib-ng/zlib-ng.h"
#include <array>
#include <queue>

namespace SleepyDiscord {
	class ZLibCompression : public GenericCompression {
	public:
		bool uncompress(const std::string& compressed, std::string& uncompressedOut) override {
			auto stream = zng_stream{};
			memset(&stream, 0, sizeof(stream));
			int output = zng_inflateInit(&stream);
			if (output != Z_OK) {
				zng_inflateEnd(&stream);
				return false;
			}

			stream.next_in = reinterpret_cast<const uint8_t*>(compressed.data());
			stream.avail_in = static_cast<uint32_t>(compressed.length());

			constexpr size_t chunkSize = 16 * 1024;
			using Data = std::array<char, chunkSize>;
			using Buffer = std::pair<Data, std::size_t>;
			std::list<Buffer> outputQueue;

			bool makeNewBuffer = true;
			std::size_t totalSize = 0;
			output = Z_BUF_ERROR;
			do {
				if (makeNewBuffer == true) {
					outputQueue.emplace_back(); //make a new output buffer
					makeNewBuffer = false;
				}
				Buffer& buffer = outputQueue.back();
				Data& data = buffer.first;
				std::size_t size = buffer.second;

				stream.next_out = reinterpret_cast<uint8_t*>(&data[size]);
				stream.avail_out = static_cast<uint32_t>(data.max_size() - size);

				output = zng_inflate(&stream, Z_SYNC_FLUSH);

				auto oldSize = size;
				size = data.max_size() - stream.avail_out;
				buffer.second = size;
				auto deltaSize = size - oldSize;
				totalSize += deltaSize;

				if (output == Z_STREAM_END) {
					output = zng_inflateEnd(&stream);
					break;
				} else if (deltaSize == 0) {
					makeNewBuffer = true;
				}
			} while (output == Z_OK || output == Z_BUF_ERROR);
			
			uncompressedOut.clear();
			uncompressedOut.reserve(totalSize);
			for (; 0 < outputQueue.size(); outputQueue.pop_front()) {
				Buffer& buffer = outputQueue.front();
				Data& data = buffer.first;
				std::size_t size = buffer.second;

				uncompressedOut.append(data.data(), size);
			}

			return true;
		}
	};

	using DefaultCompression = ZLibCompression;
#define SLEEPY_DEFAULT_COMPRESSION ZLibCompression
}