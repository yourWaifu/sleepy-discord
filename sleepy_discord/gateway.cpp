#include "gateway.h"

namespace SleepyDiscord {
	//Ready::Ready(const std::string * rawJSON) : Ready(json::getValues(rawJSON->c_str(), fields)) {}

	//Ready::Ready(const nonstd::string_view& rawJSON) : Ready(json::getValues(rawJSON.data(), fields)) {}

	//Ready::Ready(const json::Values values) :
	//	//variable        modifier                  value               felid
	//	v              (toInt                     ( values[index(fields, "v"         )])),
	//	user           (User                      ( values[index(fields, "user"      )])),
	//	//privateChannels(JSON_getArray<DMChannel>  (&values[index(fields, "user"      )])),
	//	sessionID      (                            values[index(fields, "session_id")] ),
	//	trace          (JSON_getArray<std::string>( values[index(fields, "_trace"    )]))
	//{}

	Ready::Ready(const json::Value & json) :
		Ready(json::fromJSON<Ready>(json)) {
	}
	Ready::Ready(const nonstd::string_view & json) :
		Ready(json::fromJSON<Ready>(json)) {
	}

	//const std::initializer_list<const char*const> Ready::fields = {
	//	"v", "user", /*"private_channels",*/ "guilds", "session_id", "_trace"
	//};
}