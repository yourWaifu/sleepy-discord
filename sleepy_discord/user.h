#pragma once
#include <string>
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct User : public DiscordObject {
		~User();
		User();
		User(const std::string * rawJSON, const unsigned int startPosition = 0);
		void fillOut(JSON_object * _JSON_object);
		void fillOut(const char* name, void * value);
		std::string id;
		std::string username;
		std::string discriminator;
		std::string avatar;
		bool bot = false;
		bool verified = false;
		std::string email = "";
	};
}