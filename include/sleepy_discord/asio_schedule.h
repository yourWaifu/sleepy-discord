#pragma once
#include "asio_include.h"
#include "timer.h"

namespace SleepyDiscord {
	class GenericScheduleHandler {
	public:
		virtual ~GenericScheduleHandler() = default;
		virtual Timer schedule(TimedTask code, const time_t milliseconds) = 0;
	};

#ifndef NONEXISTENT_ASIO 

	class ASIOBasedScheduleHandler : public GenericScheduleHandler {
	public:
		virtual ~ASIOBasedScheduleHandler() = default;
		virtual asio::io_context& getIOContext() = 0;
	};

	class ASIOScheduleHandler : public ASIOBasedScheduleHandler {
	public:
		ASIOScheduleHandler(std::shared_ptr<asio::io_context> _io):
			io(_io) {}
		virtual ~ASIOScheduleHandler() = default;

		inline asio::io_context& getIOContext() override {
			return *io;
		}

		static void handleTimer(const asio::error_code &ec, std::function<void()>& code) {
			if (ec != asio::error::operation_aborted) {
				code();
			}
		}

		inline static Timer schedule(asio::io_context& io, TimedTask code, const time_t milliseconds) {
			auto timer = std::make_shared<asio::steady_timer>(io, asio::chrono::milliseconds(milliseconds));
			timer->async_wait(std::bind(&handleTimer, std::placeholders::_1, code));
			return Timer([timer]() {
				timer->cancel();
			});
		}

		inline Timer schedule(TimedTask code, const time_t milliseconds) override {
			return ASIOScheduleHandler::schedule(*io, std::move(code), std::move(milliseconds));
		}

		inline void run() {
			io->run();
		}

	private:
		std::shared_ptr<asio::io_context> io;
	};
#endif
}
