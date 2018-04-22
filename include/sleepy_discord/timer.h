#pragma once
#include <functional>

namespace SleepyDiscord {
	struct Timer {
	public:
		typedef std::function<void()> StopTimerFunction;
		Timer() {}
		Timer(StopTimerFunction stopTimer) : implStop(stopTimer) {}
		inline void stop() const { implStop(); }
		inline bool isValid() const { return implStop != nullptr; }
	private:
		StopTimerFunction implStop;
	};
}
