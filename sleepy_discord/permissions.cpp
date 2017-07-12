#include "permissions.h"
#include "json.h"

namespace SleepyDiscord {
	Role::Role(const std::string * rawJson) {
		std::initializer_list<const char*const> names = {
			"id", "name", "color", "hoist", "position", "permissions",
			"managed", "mentionable"
		};
		std::vector<std::string> values = json::getValues(rawJson->c_str(), names);
		id          =                                    values[index(names, "id"         )]  ;
		name        =                                    values[index(names, "name"       )]  ;
		color       =                         std::stol( values[index(names, "color"      )]) ;
		hoist       =                         getBool(   values[index(names, "hoist"      )]) ;
		position    =                         std::stol( values[index(names, "position"   )]) ;
		permissions = static_cast<Permission>(std::stoll(values[index(names, "permissions")]));
		managed     =                         std::stol( values[index(names, "managed"    )]) ;
		mantionable =                         std::stol( values[index(names, "mentionable")]) ;
	}
}