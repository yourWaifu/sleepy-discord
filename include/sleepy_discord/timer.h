#pragma once
#include <functional>

namespace SleepyDiscord {
	typedef std::function<void()> TimedTask;

	struct Timer {
	public:
		typedef std::function<void()> StopTimerFunction;
		Timer() {}
		Timer(StopTimerFunction stopTimer) :
			implStop(stopTimer) {}
		inline void stop() { implStop(); implStop = nullptr; }
		inline bool isValid() const { return implStop != nullptr; }
	private:
		StopTimerFunction implStop;
	};
}
