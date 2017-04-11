#include "user.h"

namespace SleepyDiscord {
	User::~User() {
	}

	User::User() {
	}

	User::User(const std::string * rawJSON) {
		std::vector<std::string> values = json::getValues(rawJSON->c_str(),
			{ "id", "username", "discriminator", "avatar", "bot", "mfa_enabled", "verified", "email" });
		id = values[0];
		username = values[1];
		discriminator = values[2];
		avatar = values[3];
		bot = values[4][0] == 't' ? true : false;
		mfa_enabled = values[5][0] == 't' ? true : false;
		verified = values[6][0] == 't' ? true : false;
		email = values[7];
	}
}