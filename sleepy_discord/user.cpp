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

	UserServer::UserServer(const std::string * rawJson) {
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "name", "icon", "owner", "permissions" });

		unsigned int i = -1;
		id = values[++i];
		name = values[++i];
		icon = values[++i];
		owner = getBool(values[++i]);
		permissions = static_cast<Permission>(std::stoi(values[++i]));
	}

	Connection::Connection(const std::string * rawJson) {
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "name", "type", "revoked"});

		unsigned int i = -1;
		id = values[++i];
		name = values[++i];
		type = values[++i];
		revoked = getBool(values[++i]);
	}
}