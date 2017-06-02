#include "permissions.h"
#include "json.h"

namespace SleepyDiscord {
	Role::Role(const std::string * rawJson) {
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "name", "color", "hoist", "position", "permissions",
		"managed", "mentionable" });
		id = values[0];
		name = values[1];
		color = std::stol(values[2]);
		hoist = getBool(values[3]);
		position = std::stol(values[4]);
		permissions = static_cast<Permission>(std::stoll(values[5]));
		managed = std::stol(values[6]);
		mantionable = std::stol(values[7]);
	}
}