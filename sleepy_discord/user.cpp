#include "user.h"

namespace SleepyDiscord {
	User::~User() {
	}

	User::User() {
	}

	User::User(const std::string * rawJSON) {
		const char* names[] = { "id", "username", "discriminator", "avatar", "bot", "verified", "email" };
		const unsigned int arraySize = sizeof(names) / sizeof(*names);
		std::string values[arraySize];
		json::getValues(rawJSON->c_str(), names, values, arraySize);
		id = values[0];
		username = values[1];
		discriminator = values[2];
		avatar = values[3];
		bot = values[4][0] == 't' ? true : false;
		verified = values[5][0] == 't' ? true : false;
		email = values[6];
	}
}