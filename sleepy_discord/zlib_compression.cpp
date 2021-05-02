#if defined(EXISTENT_ZLIB) || defined(EXISTENT_ZLIB_NG)
#include "zlib_compression.h"

namespace SleepyDiscord {
	ZLibCompression::ZLibCompression() {
		stream = ZLib::Stream{};
		memset(&stream, 0, sizeof(stream));
		statusCode = ZLib::inflateInitStream(&stream);
		if (statusCode != Z_OK) {
			ZLib::inflateEndStream(&stream);
		}
		if (output.empty()) //since are using back(), we need at least one buffer in the output
			output.emplace_back(); //make a new output buffer
	}

	void ZLibCompression::uncompress(const std::string& compressed) {
		std::lock_guard<std::mutex> lock(mutex);

		stream.next_in = (ZLib::ConstByte*)(compressed.c_str());
		stream.avail_in = static_cast<uint32_t>(compressed.length());

		statusCode = Z_BUF_ERROR;
		do {
			Output::Buffer& buffer = output.back();
			Output::Data& data = buffer.first;
			std::size_t& size = buffer.second;

			stream.next_out = reinterpret_cast<ZLib::Btye*>(data.data() + size);
			stream.avail_out = static_cast<uint32_t>(data.size() - size);

			statusCode = ZLib::inflateStream(&stream, Z_SYNC_FLUSH);

			auto oldSize = size;
			size = data.size() - stream.avail_out;
			auto deltaSize = size - oldSize;

			if (statusCode == Z_STREAM_END) {
				statusCode = ZLib::inflateResetStream(&stream);
			}
			else if (deltaSize == 0) { //if did anything
				if (stream.avail_out == 0)
					output.emplace_back(); //needs more memory
				else //notthing left to do for now
					break;
			}
		} while (statusCode == Z_OK || statusCode == Z_BUF_ERROR);
	}

	void ZLibCompression::getOutput(std::string& uncompressedOut) {
		std::lock_guard<std::mutex> lock(mutex);
		if (output.empty())
			return;
		std::size_t totalSize = 0;
		{
			const Output::Buffer& lastBuffer = output.back();
			const auto lastBufferSize = lastBuffer.second;
			totalSize += lastBufferSize;
			totalSize += (output.size() - 1) * Output::chunkSize; //add size of full chucks
		}

		uncompressedOut.reserve(totalSize); //allocate memory for string
		for (Output::Buffer& buffer : output) {
			Output::Data& data = buffer.first;
			const std::size_t size = buffer.second;

			uncompressedOut.append(data.data(), size);
		}

		//reset output for next getOutput
		//since as long as we are connected to Discord, Discord will send us at least a
		//heartbeat ack. So, it might be better to keep the first buffer allocated in memory
		//to avoid doing another memory allocation.
		output.resize(1);
		output.back().second = 0;
	}
}
#endif