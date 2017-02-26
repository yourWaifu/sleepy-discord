#include "common.h"

namespace SleepyDiscord {
	int stringLength(const char * string) {
		int length = 0;
		while (string[length] != 0) ++length;
		return length;
	}
}