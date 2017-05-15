#include "server.h"
#include "client.h"

namespace SleepyDiscord {
	Server::~Server() {

	}

	Server::Server(const std::string * rawJson) {
		//parse json and convert from string to type
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
			{ "id", "name", "icon", "splash", "owner_id", "region", "afk_channel_id",
			  "afk_timeout", "embed_enabled", "embed_channel_id", "verification_level",
			  "unavailable", "mfa_level", "large", "joined_at", "default_message_notifications" });

		unsigned int i = -1;
		id                            = values[++i];
		name                          = values[++i];
		icon                          = values[++i];
		splash                        = values[++i];
		owner_id                      = values[++i];
		region                        = values[++i];
		afk_channel_id                = values[++i];
		afk_timeout                   = std::stod(values[++i]);
		embed_enable                  = getBool(values[++i]);
		embed_channel_id              = values[++i];
		verfication_level             = std::stod(values[++i]);
		unavailable                   = getBool(values[++i]);
		mfa_level                     = std::stod(values[++i]);
		large                         = getBool(values[++i]);
		joined_at                     = values[++i];
		default_message_notifications = std::stod(values[++i]);
	}

	ServerEmbed::ServerEmbed(const std::string * rawJson) {
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "enabled", "channel_id" });

		unsigned int i = -1;
		enabled    = getBool(values[++i]);
		channel_id = values[++i];
	}

	ServerMember::ServerMember(const std::string * rawJson) {
		//parse json and convert from string to type
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
			{ "user", "nick", "roles", "joined_at", "deaf", "mute" });

		unsigned int i = -1;
		user      = User(&values[++i]);
		nick      = isDefined(values[++i]) ? values[i] : "";
		roles     = json::getArray(&values[++i]);
		joined_at = values[++i];
		deaf      = getBool(values[++i]);
		mute      = getBool(values[++i]);
	}

	ServerMember::ServerMember(BaseDiscordClient* client, std::string server_id, std::string user_id) {
		*this = client->getMember(server_id, user_id);
	}
}