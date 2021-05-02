#pragma once
#include "generic_compression.h"
#ifdef EXISTENT_ZLIB
	#include "zlib.h"
#elif defined(EXISTENT_ZLIB_NG)
	#include "zlib-ng/zlib-ng.h"
#endif
#include <array>
#include <forward_list>
#include <string>
#include <mutex>

namespace SleepyDiscord {
	//This Queue is basicly a single linked list with the back and size stored
	//Needed for storing the output before copying it over into one string
	struct OutputQueue {
		constexpr static size_t chunkSize = 16 * 1024;
		using Data = std::array<char, chunkSize>;
		using Buffer = std::pair<Data, std::size_t>;

		using Queue = std::forward_list<Buffer>;
		using Iterator = std::forward_list<Buffer>::iterator;
		using ConstIterator = std::forward_list<Buffer>::const_iterator;

		Queue queue; //needed when output is larger then buffer
		Iterator _back = queue.before_begin(); //both back and size would require looking for them
		Queue::size_type _size = 0; //to avoid looking for size, we store it. Same goes for back

		~OutputQueue() = default;

		bool empty() const { return queue.empty(); }

		//allocates more memory
		template<class... Args>
		Iterator emplace_back(Args&&... args) {
			Iterator result = queue.emplace_after(_back, std::forward<Args>(args)...);
			if (_back != result) { //if did anything
				_back = result;
				_size += 1;
			}
			return result;
		}

		Buffer& front() {
			return queue.front();
		}
		Buffer& back() {
			return *_back;
		}

		Iterator begin() noexcept {
			return queue.begin();
		}
		Iterator end() noexcept {
			return queue.end();
		}

		ConstIterator begin() const noexcept {
			return queue.begin();
		}
		ConstIterator end() const noexcept {
			return queue.end();
		}

		Queue::size_type size() const noexcept {
			return _size;
		}

		void resize(Queue::size_type count) {
			queue.resize(count);
			_size = 0;
			//linear time complexity but count is usally 1
			for (Iterator it = begin(); it != end(); ++it) {
				_size += 1;
				_back = it;
			}
		}
	};

#ifdef EXISTENT_ZLIB
	namespace ZLib {
		using Stream = z_stream;
		using Btye = Bytef;
		using ConstByte = z_const Bytef;
		inline int inflateInitStream(Stream* stream) { return inflateInit(stream); }
		inline int inflateEndStream(Stream* stream) { return inflateEnd(stream); }
		inline int inflateResetStream(Stream* stream) { return inflateReset(stream); }
		inline int inflateStream(Stream* stream, int mode) { return inflate(stream, mode); }
	}
#elif defined(EXISTENT_ZLIB_NG)
	namespace ZLib {
		using Stream = zng_stream;
		using Btye = uint8_t;
		using ConstByte = const uint8_t;
		inline int inflateInitStream(Stream* stream) { return zng_inflateInit(stream); }
		inline int inflateEndStream(Stream* stream) { return zng_inflateEnd(stream); }
		inline int inflateResetStream(Stream* stream) { return zng_inflateReset(stream); }
		inline int inflateStream(Stream* stream, int mode) { return zng_inflate(stream, mode); }
	}
#endif

	class ZLibCompression : public GenericCompression {
	public:
		using Output = OutputQueue;

		ZLibCompression();

		~ZLibCompression() {
			ZLib::inflateEndStream(&stream);
		}

		ZLib::Stream stream;
		int statusCode;

		Output output;
		std::mutex mutex; //only allow one thread to uncompress

		void uncompress(const std::string& compressed) override;
		void getOutput(std::string& uncompressedOut) override;

		inline void resetStream() override {
			ZLib::inflateResetStream(&stream);
		}

		inline bool streamEnded() override {
			return statusCode == Z_STREAM_END;
		}
	};

	using DefaultCompression = ZLibCompression;
#define SLEEPY_DEFAULT_COMPRESSION ZLibCompression
}