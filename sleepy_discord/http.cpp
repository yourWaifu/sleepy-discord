#include <cctype>
#include "http.h"

namespace SleepyDiscord {
	//copied from cpr
	bool caseInsensitiveCompare::operator()(const std::string& a, const std::string& b) const noexcept {
		return std::lexicographical_compare(
			a.begin(), a.end(), b.begin(), b.end(),
			[](unsigned char ac, unsigned char bc) { return std::tolower(ac) < std::tolower(bc); });
	}

	const char * GenericSession::getMethodName(const RequestMethod & method) {
		static constexpr char const * methodNames[] = { "POST", "PATCH", "DELETE", "GET", "PUT" };
		return methodNames[method];
	}
}
