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
		virtual asio::io_service& getIOService() = 0;
	};

	class ASIOScheduleHandler : public ASIOBasedScheduleHandler {
	public:
		ASIOScheduleHandler() {}
		virtual ~ASIOScheduleHandler() = default;

		inline asio::io_service& getIOService() override {
			return io;
		}

		static void handleTimer(const asio::error_code &ec, std::function<void()>& code) {
			if (ec != asio::error::operation_aborted) {
				code();
			}
		}

		inline Timer schedule(TimedTask code, const time_t milliseconds) override {
			auto timer = std::make_shared<asio::steady_timer>(io, asio::chrono::milliseconds(milliseconds));
			timer->async_wait(std::bind(&handleTimer, std::placeholders::_1, code));
			return Timer([timer]() {
				timer->cancel();
			});
		}

		inline void run() {
			io.run();
		}

	private:
		asio::io_service io;
	};
#endif
}
