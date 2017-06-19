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
		bot = getBool(values[4]);
		mfa_enabled = getBool(values[5]);
		verified = getBool(values[6]);
		email = values[7];
	}

	bool User::operator==(const User& rightUser) {
		return id == rightUser.id;
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