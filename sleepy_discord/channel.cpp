#include "channel.h"

namespace SleepyDiscord {
	Channel::Channel(const std::string * rawJSON) : Channel(json::getValues(rawJSON->c_str(), fields)) {}

	Channel::Channel(const std::vector<std::string> values) : 
		//felid                modifier                       value
		Parent               (                                values[index(fields, "id"                   )]  ),
		type                 (static_cast<ChannelType>(toInt( values[index(fields, "type"                 )]))),
		serverID             (                                values[index(fields, "guild_id"             )]  ),
		position             (toInt                         ( values[index(fields, "position"             )]) ),
		permissionOverwrites (modIfElse(isSpecified, JSON_getArray<Overwrite>, values[index(fields, "permission_overwrites" )], std::vector<Overwrite>())),
		name                 (                                values[index(fields, "name"                 )]  ),
		topic                (                                values[index(fields, "topic"                )]  ),
		isNSFW               (getBool                       ( values[index(fields, "nsfw"                 )]) ),
		lastMessageID        (                                values[index(fields, "last_message_id"      )]  ),
		bitrate              (toInt                         ( values[index(fields, "bitrate"              )]) ),
		userLimit            (toInt                         ( values[index(fields, "user_limit"           )]) ),
		//recipients
		icon                 (                                values[index(fields, "icon"                 )]  ),
		ownerID              (                                values[index(fields, "owner_id"             )]  ),
		//applicationID
		parentID             (                                values[index(fields, "parent_id"            )]  ),
		lastPinTimestamp     (                                values[index(fields, "last_pin_timestamp"   )]  )
	{}

	const std::initializer_list<const char*const> Channel::fields = {
		"id", "type", "guild_id", "position", "permission_overwrites",
		"name", "topic", "nsfw", "last_message_id", "bitrate",
		"user_limit", "recipients", "icon", "owner_id",
		"application_id", "parent_id", "last_pin_timestamp"
	};

	Channel::~Channel() {

	}

	DirectMessageChannel::~DirectMessageChannel() {

	}

	DirectMessageChannel::DirectMessageChannel(const std::string * rawJSON) : DirectMessageChannel(json::getValues(rawJSON->c_str(), fields)) {}

	DirectMessageChannel::DirectMessageChannel(const std::vector<std::string> values) :
		//variable       modifier value                    felid
		ID             (         values[index(fields, "id"             )] ),
		is_private     ( getBool(values[index(fields, "is_private"     )])),
		lastMessageID  (         values[index(fields, "last_message_id")] )
	{}

	const std::initializer_list<const char*const> DirectMessageChannel::fields = {
		"id", "is_private", "recipient", "last_message_id"
	};

	Overwrite::Overwrite() : allow(Permission::NONE), deny(Permission::NONE)
	{}

	Overwrite::Overwrite(const std::string * rawJSON) : Overwrite(json::getValues(rawJSON->c_str(), fields)) {}

	Overwrite::Overwrite(const std::vector<std::string> values) :
		//variable modifier value              felid
		Parent(      values[index(fields, "id"   )] ),
		type  (      values[index(fields, "type" )] ),
		allow (toPermission(toLongLong(values[index(fields, "allow")]))),
		deny  (toPermission(toLongLong(values[index(fields, "deny" )])))
	{}
	const std::initializer_list<const char*const> Overwrite::fields = {
		"id", "type", "allow", "deny"
	};
}
