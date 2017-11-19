#include "server.h"
#include "client.h"

namespace SleepyDiscord {
	Server::~Server() {

	}

	Server::Server() {

	}

	Server::Server(const std::string * rawJSON) : Server(json::getValues(rawJSON->c_str(), fields)) {}

	Server::Server(const std::vector<std::string> values) :
		//variable                            condition    modifier                value        felid                                    else
		ID                         (                                               values[index(fields, "id"                           )]                         ),
		name                       (                                               values[index(fields, "name"                         )]                         ),
		icon                       (                                               values[index(fields, "icon"                         )]                         ),
		splash                     (                                               values[index(fields, "splash"                       )]                         ),
		ownerID                    (                                               values[index(fields, "owner_id"                     )]                         ),
		region                     (                                               values[index(fields, "region"                       )]                         ),
		AFKchannelID               (                                               values[index(fields, "afk_channel_id"               )]                         ),
		afk_timeout                (                       toInt                  (values[index(fields, "afk_timeout"                  )]                        )),
		embedEnable                (                       getBool                (values[index(fields, "embed_enabled"                )]                        )),
		embedChannelID             (                                               values[index(fields, "embed_channel_id"             )]                         ),
		verficationLevel           (                       toInt                  (values[index(fields, "verification_level"           )]                        )),
		defaultMessageNotifications(                       toInt                  (values[index(fields, "default_message_notifications")]                        )),
		unavailable                (                       getBool                (values[index(fields, "unavailable"                  )]                        )),
		MFALevel                   (                       toInt                  (values[index(fields, "mfa_level"                    )]                        )),
		joinedAt                   (                                               values[index(fields, "joined_at"                    )]                         ),
		large                      (                       getBool                (values[index(fields, "large"                        )]                        )),
		channels                   (modIfElse(isSpecified, JSON_getArray<Channel>, values[index(fields, "channels"                     )], std::vector<Channel>()))
	{}

	const std::initializer_list<const char*const> Server::fields = {
		"id", "name", "icon", "splash", "owner_id", "region", "afk_channel_id",
		"afk_timeout", "embed_enabled", "embed_channel_id", "verification_level",
		"unavailable", "mfa_level", "large", "joined_at", "default_message_notifications",
		/*"Servers", "emojis", "features", "explicit_content_filter", application_id,
		widget_enabled, widget_channel_id, member_count, voice_states, members, presences*/
		"channels"
	};

	ServerEmbed::ServerEmbed(const std::string * rawJSON) : ServerEmbed(json::getValues(rawJSON->c_str(), fields)) {}

	ServerEmbed::ServerEmbed(const std::vector<std::string> values) :
		//variable modifier value              felid
		enabled  (getBool(values[index(fields, "enabled"   )])),
		channelID(        values[index(fields, "channel_id")] )
	{}

	const std::initializer_list<const char*const> ServerEmbed::fields = {
		"enabled", "channel_id"
	};

	ServerMember::ServerMember(const std::string * rawJSON) : ServerMember(json::getValues(rawJSON->c_str(), fields)) {}

	ServerMember::ServerMember(const std::vector<std::string> values) :
		//variable         condition modifier        value                felid         else
		user    (                    User          (&values[index(fields, "user"     )]    )),
		nick    (setIfElse(isDefined,                values[index(fields, "nick"     )], "")),
		roles   (                    json::getArray(&values[index(fields, "roles"    )]    )),
		joinedAt(                                    values[index(fields, "joined_at")]     ),
		deaf    (                    getBool        (values[index(fields, "deaf"     )]    )),
		mute    (                    getBool        (values[index(fields, "mute"     )]    ))
	{}

	const std::initializer_list<const char*const> ServerMember::fields = {
		"user", "nick", "roles", "joined_at", "deaf", "mute"
	};
}