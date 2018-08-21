#include "server.h"
#include "client.h"
#include "permissions.h"

namespace SleepyDiscord {
	ServerMember::ServerMember() : deaf(false), mute(false) {}

	ServerMember::ServerMember(const std::string * rawJSON) : ServerMember(json::getValues(rawJSON->c_str(), fields)) {}

	ServerMember::ServerMember(const std::vector<std::string> values) :
		//variable         condition modifier                       value                felid         else
		user    (                    User                         (&values[index(fields, "user"     )]    )),
		nick    (setIfElse(isDefined,                               values[index(fields, "nick"     )], "")),
		roles   (                    JSON_getArray<Snowflake<Role>>(values[index(fields, "roles"    )]    )),
		joinedAt(                                                   values[index(fields, "joined_at")]     ),
		deaf    (                    getBool                       (values[index(fields, "deaf"     )]    )),
		mute    (                    getBool                       (values[index(fields, "mute"     )]    ))
	{
		Parent(user.ID);
	}

	const std::initializer_list<const char*const> ServerMember::fields = {
		"user", "nick", "roles", "joined_at", "deaf", "mute"
	};

	Server::~Server() {

	}

	Server::Server() {

	}

	Server::Server(const std::string * rawJSON) : Server(json::getValues(rawJSON->c_str(), fields)) {}

	Server::Server(const std::vector<std::string> values) :
		//variable                            condition    modifier                     value        felid                                    else
		Parent                     (                                                    values[index(fields, "id"                           )]                              ),
		name                       (                                                    values[index(fields, "name"                         )]                              ),
		icon                       (                                                    values[index(fields, "icon"                         )]                              ),
		splash                     (                                                    values[index(fields, "splash"                       )]                              ),
		ownerID                    (                                                    values[index(fields, "owner_id"                     )]                              ),
		permissions                (                       toPermission(toLongLong     (values[index(fields, "permissions"                  )]                            ))),
		region                     (                                                    values[index(fields, "region"                       )]                              ),
		AFKchannelID               (                                                    values[index(fields, "afk_channel_id"               )]                              ),
		AFKTimeout                 (                       toInt                       (values[index(fields, "afk_timeout"                  )]                             )),
		embedEnable                (                       getBool                     (values[index(fields, "embed_enabled"                )]                             )),
		embedChannelID             (                                                    values[index(fields, "embed_channel_id"             )]                              ),
		verficationLevel           (                       toInt                       (values[index(fields, "verification_level"           )]                             )),
		defaultMessageNotifications(                       toInt                       (values[index(fields, "default_message_notifications")]                             )),
		roles                      (                       JSON_getArray<Role>         (values[index(fields, "roles"                        )]                             )),
		unavailable                (                       getBool                     (values[index(fields, "unavailable"                  )]                             )),
		MFALevel                   (                       toInt                       (values[index(fields, "mfa_level"                    )]                             )),
		joinedAt                   (                                                    values[index(fields, "joined_at"                    )]                              ),
		large                      (                       getBool                     (values[index(fields, "large"                        )]                             )),
		members                    (modIfElse(isSpecified, JSON_getArray<ServerMember>, values[index(fields, "members"                      )], std::vector<ServerMember>())),
		channels                   (modIfElse(isSpecified, JSON_getArray<Channel>,      values[index(fields, "channels"                     )], std::vector<Channel>()     ))
	{}

	std::vector<ServerMember>::iterator Server::findMember(Snowflake<User> userID) {
		return userID.findObject(members.begin(), members.end());
	}

	std::vector<Channel>::iterator Server::findChannel(Snowflake<Channel> channelID) {
		return channelID.findObject(channels.begin(), channels.end());
	}

	const std::initializer_list<const char*const> Server::fields = {
		"id", "name", "icon", "splash", "owner_id", "permissions", "region",
		"afk_channel_id", "afk_timeout", "embed_enabled", "embed_channel_id",
		"verification_level", "unavailable", "mfa_level", "large", "joined_at",
		"default_message_notifications", /*"Servers", "emojis", "features",
		"explicit_content_filter", application_id, widget_enabled, widget_channel_id,
		member_count, voice_states, presences*/ "roles", "members", "channels"
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

	UnavailableServer::UnavailableServer(const std::string * rawJSON) : UnavailableServer(json::getValues(rawJSON->c_str(), fields)) {}

	UnavailableServer::UnavailableServer(const std::vector<std::string> values) :
		//variable  modifier value               felid
		Parent     (        values[index(fields, "id")]),
		unavailable(getBool(values[index(fields, "unavailable")])) {
	}

	const std::initializer_list<const char*const> UnavailableServer::fields = {
		"id", "unavailable"
	};
}