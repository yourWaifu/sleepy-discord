#include "channel.h"

namespace SleepyDiscord {
	Channel::Channel(const std::string * rawJSON) : Channel(json::getValues(rawJSON->c_str(), fields)) {}

	Channel::Channel(const std::vector<std::string> values) :
		//felid                modifier                        value
		ID                   (                                 values[index(fields, "id"                   )]  ),
		serverID             (                                 values[index(fields, "guild_id"             )]  ),
		name                 (                                 values[index(fields, "name"                 )]  ),
		type                 ( static_cast<ChannelType>(toInt( values[index(fields, "type"                 )]))),
		position             ( toInt                         ( values[index(fields, "position"             )]) ),
		isPrivate            ( getBool                       ( values[index(fields, "is_private"           )]) ),
		//permissionOverwrites ( Overwrite                     (&values[index(fields, "permission_overwrites")]) ),
		topic                (                                 values[index(fields, "topic"                )]  ),
		lastMessageID       (                                 values[index(fields, "last_message_id"      )]  ),
		bitrate              ( toInt                         ( values[index(fields, "bitrate"              )]) ),
		userLimit            ( toInt                         ( values[index(fields, "user_limit"           )]) )
	{}

	const std::initializer_list<const char*const> Channel::fields = {
		"id", "guild_id", "name", "type", "position", "is_private",
		/*"permission_overwrites",*/ "topic", "last_message_id", "bitrate", "user_limit"
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
		recipient      ( User(  &values[index(fields, "recipient"      )])),
		lastMessageID  (         values[index(fields, "last_message_id")] )
	{}

	const std::initializer_list<const char*const> DirectMessageChannel::fields = {
		"id", "is_private", "recipient", "last_message_id"
	};

	Overwrite::Overwrite(const std::string * rawJSON) : Overwrite(json::getValues(rawJSON->c_str(), fields)) {}

	Overwrite::Overwrite(const std::vector<std::string> values) :
		//variable modifier value              felid
		ID    (      values[index(fields, "id"   )] ),
		type  (      values[index(fields, "type" )] ),
		allow (toInt(values[index(fields, "allow")])),
		deny  (toInt(values[index(fields, "deny" )]))
	{}
	const std::initializer_list<const char*const> Overwrite::fields = {
		"id", "type", "allow", "deny"
	};
}