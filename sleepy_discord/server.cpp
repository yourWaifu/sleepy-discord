#include "server.h"
#include "client.h"

namespace SleepyDiscord {
	Server::~Server() {

	}

	Server::Server() {

	}

	Server::Server(const std::string * rawJson) {
		//parse json and convert from string to type
		std::initializer_list<const char*const> names = {
			"id", "name", "icon", "splash", "owner_id", "region", "afk_channel_id",
			"afk_timeout", "embed_enabled", "embed_channel_id", "verification_level",
			"unavailable", "mfa_level", "large", "joined_at", "default_message_notifications",
			/*"roles", "emojis", "features", "explicit_content_filter", application_id,
			widget_enabled, widget_channel_id, member_count, voice_states, members, presences*/
			"channels"
		};
		std::vector<std::string> values = json::getValues(rawJson->c_str(), names);

		//    condition    variable                      modifier                  value               felid
		                   id                            =                         values[index(names, "id"                           )] ;
		                   name                          =                         values[index(names, "name"                         )] ;
		                   icon                          =                         values[index(names, "icon"                         )] ;
		                   splash                        =                         values[index(names, "splash"                       )] ;
		                   owner_id                      =                         values[index(names, "owner_id"                     )] ;
		                   region                        =                         values[index(names, "region"                       )] ;
		                   afk_channel_id                =                         values[index(names, "afk_channel_id"               )] ;
		                   afk_timeout                   = toInt                  (values[index(names, "afk_timeout"                  )]);
		                   embed_enable                  = getBool                (values[index(names, "embed_enabled"                )]);
		                   embed_channel_id              =                         values[index(names, "embed_channel_id"             )] ;
		                   verfication_level             = toInt                  (values[index(names, "verification_level"           )]);
		                   unavailable                   = getBool                (values[index(names, "unavailable"                  )]);
		                   mfa_level                     = toInt                  (values[index(names, "mfa_level"                    )]);
		                   large                         = getBool                (values[index(names, "large"                        )]);
		                   joined_at                     =                         values[index(names, "joined_at"                    )] ;
		                   default_message_notifications = toInt                  (values[index(names, "default_message_notifications")]);
		modIf(isSpecified, channels                      , JSON_getArray<Channel>, values[index(names, "channels"                     )]);
	}

	ServerEmbed::ServerEmbed(const std::string * rawJson) {
		std::initializer_list<const char*const> names = {
			"enabled", "channel_id"
		};
		std::vector<std::string> values = json::getValues(rawJson->c_str(), names);

		enabled    = getBool(values[index(names, "enabled"   )]);
		channel_id =         values[index(names, "channel_id")] ;
	}

	ServerMember::ServerMember(const std::string * rawJson) {
		//parse json and convert from string to type
		std::initializer_list<const char*const> names = {
			"user", "nick", "roles", "joined_at", "deaf", "mute"
		};
		std::vector<std::string> values = json::getValues(rawJson->c_str(), names);

		//        condition  felid       modifier         value               felid        else
		                     user      = User(           &values[index(names, "user"     )]    );
		setIfElse(isDefined, nick      ,                  values[index(names, "nick"     )], "");
		                     roles     = json::getArray( &values[index(names, "roles"    )]    );
		                     joined_at =                  values[index(names, "joined_at")]     ;
		                     deaf      = getBool(         values[index(names, "deaf"     )]    );
		                     mute      = getBool(         values[index(names, "mute"     )]    );
	}

	ServerMember::ServerMember(BaseDiscordClient* client, std::string server_id, std::string user_id) {
		*this = client->getMember(server_id, user_id);
	}
}