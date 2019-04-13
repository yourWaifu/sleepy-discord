#include "permissions.h"
#include "json.h"
#include "server.h"
#include "channel.h"

namespace SleepyDiscord {
	Role::Role(const std::string * rawJSON) : Role(json::getValues(rawJSON->c_str(), fields)) {}

	Role::Role(const std::vector<std::string> values) :
		//variable  modifier                           value                     felid
		Parent     (                                   values[index(fields, "id"         )]  ),
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

	Permission getBasePermissions(const Server& server, const ServerMember& member) {
		if (server.ownerID == member.user.ID)
			return Permission::ALL;

		auto everyoneRole = static_cast<Snowflake<Role>>(server.ID).findObject(server.roles);

		if (everyoneRole == server.roles.end())
			return Permission::NONE;

		Permission permissions = everyoneRole->permissions;

		for (Snowflake<Role> roleID : member.roles) {
			auto rolePermissions = roleID.findObject(server.roles);
			if (rolePermissions != server.roles.end())
				permissions = permissions | rolePermissions->permissions;
		}

		if (hasPermission(permissions, Permission::ADMINISTRATOR))
			return Permission::ALL;
		return permissions;
	}

	void handleOverwrite(Permission& target, const Permission& allow, const Permission& deny) {
		target = (target & static_cast<Permission>(~deny)) | allow;
	}

	void handleOverwrite(Permission& target, const Overwrite& overwrite) {
		handleOverwrite(target, overwrite.allow, overwrite.deny);
	}

	Permission overwritePermissions(const Permission basePermissions, const Server& server, const ServerMember& member, const Channel& channel) {
		if (hasPermission(basePermissions, Permission::ADMINISTRATOR))
			return Permission::ALL;

		Permission permissions = basePermissions;
		const std::vector<Overwrite>& overwrites = channel.permissionOverwrites;

		auto everyoneOverwrites = static_cast<Snowflake<Overwrite>>(server.ID).findObject(overwrites);
		if (everyoneOverwrites != overwrites.end())
			handleOverwrite(permissions, *everyoneOverwrites);

		//role overwrites
		Permission allow = NONE;
		Permission deny = NONE;
		for (Snowflake<Role> roleID : member.roles) {
			auto roleOverwrite = static_cast<Snowflake<Overwrite>>(roleID).findObject(overwrites);
			if (roleOverwrite != overwrites.end()) {
				allow = allow | roleOverwrite->allow;
				deny = deny | roleOverwrite->deny;
			}
		}

		handleOverwrite(permissions, allow, deny);

		//member overwrites
		auto memberOverwrites = static_cast<Snowflake<Overwrite>>(member.user.ID).findObject(overwrites);
		if (memberOverwrites != overwrites.end())
			handleOverwrite(permissions, *memberOverwrites);

		return permissions;
	}

	Permission getPermissions(const Server& server, const ServerMember& member, const Channel& channel) {
		return overwritePermissions(getBasePermissions(server, member), server, member, channel);
	}
}