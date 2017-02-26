#pragma once
#include <string>
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct User : public DiscordObject {
		~User();
		User();
		User(const std::string * rawJSON);
		std::string id;
		std::string username;
		std::string discriminator;
		std::string avatar;
		bool bot = false;
		bool verified = false;
		std::string email = "";
	};
}