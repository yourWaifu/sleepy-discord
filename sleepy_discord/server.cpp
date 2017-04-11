#include "server.h"

namespace SleepyDiscord {
	Server::~Server() {

	}

	Server::Server(const std::string * rawJson) {
		//parse json and convert from string to type
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
			{ "id", "name", "icon", "splash", "owner_id", "region", "afk_channel_id",
			  "afk_timeout", "embed_enabled", "embed_channel_id", "verification_level",
			  "unavailable", "mfa_level", "large", "joined_at", "default_message_notifications" });

		id = values[0];
		name = values[1];
		icon = values[2];
		splash = values[3];
		owner_id = values[4];
		region = values[5];
	}
}