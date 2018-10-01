#include "gateway.h"

namespace SleepyDiscord {
	Ready::Ready(const json::Value & json) :
		Ready(json::fromJSON<Ready>(json)) {
	}
	Ready::Ready(const nonstd::string_view & json) :
		Ready(json::fromJSON<Ready>(json)) {
	}
}