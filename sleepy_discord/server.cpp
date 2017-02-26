#include "server.h"

namespace SleepyDiscord {
	Server::~Server() {

	}

	Server::Server(const std::string * rawJson) {
		//parse json and convert from string to type
		const char * names[] = { "id", "name", "icon", "splash", "owner_id", "region", "afk_channel_id",
								 "afk_timeout", "embed_enabled", "embed_channel_id", "verification_level",
								 "unavailable", "mfa_level", "large", "joined_at", "default_message_notifications" };
		constexpr unsigned int arraySize = sizeof(names) / sizeof(*names);
		std::string values[arraySize];
		json::getValues(rawJson->c_str(), names, values, arraySize);

		id = values[0];
		name = values[1];
		icon = values[2];
		splash = values[3];
		owner_id = values[4];
		region = values[5];
	}
}