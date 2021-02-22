#include "zlib-ng_compression.h"

namespace SleepyDiscord {
	ZLibCompression::ZLibCompression() {
		stream = zng_stream{};
		memset(&stream, 0, sizeof(stream));
		statusCode = zng_inflateInit(&stream);
		if (statusCode != Z_OK) {
			zng_inflateEnd(&stream);
		}
	}

	void ZLibCompression::uncompress(const std::string& compressed) {
		std::lock_guard<std::mutex> lock(mutex);
		stream.next_in = reinterpret_cast<const uint8_t*>(compressed.c_str());
		stream.avail_in = static_cast<uint32_t>(compressed.length());

		outputQueue.emplace_back(); //make a new output buffer

		statusCode = Z_BUF_ERROR;
		do {
			Buffer& buffer = outputQueue.back();
			Data& data = buffer.first;
			std::size_t& size = buffer.second;

			stream.next_out = reinterpret_cast<uint8_t*>(data.data() + size);
			stream.avail_out = static_cast<uint32_t>(data.max_size() - size);

			statusCode = zng_inflate(&stream, Z_SYNC_FLUSH);

			auto oldSize = size;
			size = data.max_size() - stream.avail_out;
			auto deltaSize = size - oldSize;

			if (statusCode == Z_STREAM_END) {
				break;
			}
			else if (deltaSize == 0) {
				if (stream.avail_out == 0)
					outputQueue.emplace_back(); //needs more memory
				else
					break;
			}
		} while (statusCode == Z_OK || statusCode == Z_BUF_ERROR);
	}

	void ZLibCompression::getOutput(std::string& uncompressedOut) {
		std::lock_guard<std::mutex> lock(mutex);
		if (outputQueue.empty())
			return;
		std::size_t totalSize = 0;
		{
			Buffer& lastBuffer = outputQueue.back();
			const auto lastBufferSize = lastBuffer.second;
			totalSize += lastBufferSize;
			totalSize += (outputQueue.size() - 1) * chunkSize; //add size of full chucks
		}

		uncompressedOut.reserve(totalSize);
		for (; 0 < outputQueue.size(); outputQueue.pop_front()) {
			Buffer& buffer = outputQueue.front();
			Data& data = buffer.first;
			const std::size_t size = buffer.second;

			uncompressedOut.append(data.data(), size);
		}

		//clear output for next getOutput
		outputQueue.clear();
	}
}