#include "permissions.h"
#include "json.h"

SleepyDiscord::Role::Role(const std::string * rawJson) {
	std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "name", "color", "hoist", "position", "permissions",
		"managed", "mentionable"});
	id = values[0];
	name = values[1];
	color = std::stol(values[2]);
	hoist = values[3][0] == 't';
	position = std::stol(values[4]);
	permissions = static_cast<Permission>(std::stoll(values[5]));
	managed = values[6][0] == 't';
	mantionable = values[7][0] == 't';
}
