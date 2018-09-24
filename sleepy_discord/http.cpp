#include "http.h"

namespace SleepyDiscord {
	const char * GenericSession::getMethodName(const RequestMethod & method) {
		static constexpr char const * methodNames[] = { "POST", "PATCH", "DELETE", "GET", "PUT" };
		return methodNames[method];
	}
}
