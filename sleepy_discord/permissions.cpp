#include "permissions.h"
#include "json.h"

namespace SleepyDiscord {
	Role::Role() : color(0), hoist(false), position(0), permissions(NONE), managed(0),
		mantionable(false)
	{}

	Role::Role(const std::string * rawJSON) : Role(json::getValues(rawJSON->c_str(), fields)) {}

	Role::Role(const std::vector<std::string> values) :
		//variable  modifier                           value                     felid
		ID         (                                   values[index(fields, "id"         )]  ),
		name       (                                   values[index(fields, "name"       )]  ),
		color      (                        std::stol (values[index(fields, "color"      )]) ),
		hoist      (                        getBool   (values[index(fields, "hoist"      )]) ),
		position   (                        std::stol (values[index(fields, "position"   )]) ),
		permissions(static_cast<Permission>(std::stoll(values[index(fields, "permissions")]))),
		managed    (                        getBool   (values[index(fields, "managed"    )]) ),
		mantionable(                        getBool   (values[index(fields, "mentionable")]) )
	{}

	const std::initializer_list<const char*const> Role::fields = {
		"id", "name", "color", "hoist", "position", "permissions",
		"managed", "mentionable"
	};
}