#include "user.h"

namespace SleepyDiscord {
	User::~User() {
	}

	User::User() {
	}

	User::User(const std::string * rawJSON) {
		std::initializer_list<const char*const> names = {
			"id", "username", "discriminator", "avatar", "bot", "mfa_enabled", "verified", "email"
		};
		std::vector<std::string> values = json::getValues(rawJSON->c_str(), names);
		id            =         values[index(names, "id"           )] ;
		username      =         values[index(names, "username"     )] ;
		discriminator =         values[index(names, "discriminator")] ;
		avatar        =         values[index(names, "avatar"       )] ;
		bot           = getBool(values[index(names, "bot"          )]);
		mfa_enabled   = getBool(values[index(names, "mfa_enabled"  )]);
		verified      = getBool(values[index(names, "verified"     )]);
		email         =         values[index(names, "email"        )] ;
	}

	bool User::operator==(const User& rightUser) {
		return id == rightUser.id;
	}

	UserServer::UserServer(const std::string * rawJson) {
		std::initializer_list<const char*const> names = {
			"id", "name", "icon", "owner", "permissions"
		};
		std::vector<std::string> values = json::getValues(rawJson->c_str(), names);
		id          =                                   values[index(names, "id"         )]  ;
		name        =                                   values[index(names, "name"       )]  ;
		icon        =                                   values[index(names, "icon"       )]  ;
		owner       = getBool(                          values[index(names, "owner"      )]) ;
		permissions = static_cast<Permission>(std::stoi(values[index(names, "permissions")]));
	}

	Connection::Connection(const std::string * rawJson) {
		std::initializer_list<const char*const> names = {
			"id", "name", "type", "revoked"
		};
		std::vector<std::string> values = json::getValues(rawJson->c_str(), names);
		id      =         values[index(names, "id"     )] ;
		name    =         values[index(names, "name"   )] ;
		type    =         values[index(names, "type"   )] ;
		revoked = getBool(values[index(names, "revoked")]);
	}
}