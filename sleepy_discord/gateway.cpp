#include "gateway.h"

namespace SleepyDiscord {
	Ready::Ready(const std::string * rawJSON) : Ready(json::getValues(rawJSON->c_str(), fields)) {}

	Ready::Ready(const std::vector<std::string> values) :
		//variable        modifier                  value               felid
		v              (toInt                     ( values[index(fields, "v"         )])),
		user           (User                      (&values[index(fields, "user"      )])),
		//privateChannels(JSON_getArray<DMChannel>  (&values[index(fields, "user"      )])),
		sessionID      (                            values[index(fields, "session_id")] ),
		trace          (JSON_getArray<std::string>(&values[index(fields, "_trace"    )]))
	{}

	const std::initializer_list<const char*const> Ready::fields = {
		"v", "user", /*"private_channels",*/ "guilds", "session_id", "_trace"
	};
}