#include "gateway.h"

namespace SleepyDiscord {
	Ready::Ready(const std::string * rawJSON) {
		std::initializer_list<const char*const> names = {
			"v", "user", /*"private_channels",*/ "guilds", "session_id", "_trace"
		};
		std::vector<std::string> values = json::getValues(rawJSON->c_str(), names);

		//variable        modifier                    value               felid
		v               = toInt                     ( values[index(names, "v"         )]);
		user            = User                      (&values[index(names, "user"      )]);
		//privateChannels = JSON_getArray<DMChannel>  (&values[index(names, "user"      )]);
		session_id      =                             values[index(names, "session_id")] ;
		trace           = JSON_getArray<std::string>(&values[index(names, "user"      )]);
	}
}