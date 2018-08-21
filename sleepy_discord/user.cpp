#include "user.h"

namespace SleepyDiscord {
	User::~User() {
	}

	User::User() {
	}

	const std::initializer_list<const char*const> User::fields = {
		"id", "username", "discriminator", "avatar", "bot", "mfa_enabled", "verified", "email"
	};

	User::User(const std::string * rawJSON) : User(json::getValues(rawJSON->c_str(), fields)) {}

	User::User(const std::vector<std::string> values) :
		//variable     modifier value               felid
		Parent        (        values[index(fields, "id"           )] ),
		username      (        values[index(fields, "username"     )] ),
		discriminator (        values[index(fields, "discriminator")] ),
		avatar        (        values[index(fields, "avatar"       )] ),
		bot           (getBool(values[index(fields, "bot"          )])),
		mfa_enabled   (getBool(values[index(fields, "mfa_enabled"  )])),
		verified      (getBool(values[index(fields, "verified"     )])),
		email         (        values[index(fields, "email"        )] )
	{}

	ServerUser::ServerUser() : owner(false), permissions(NONE)
	{}
	
	ServerUser::ServerUser(const std::string * rawJSON) : ServerUser(json::getValues(rawJSON->c_str(), fields)) {}

	ServerUser::ServerUser(const std::vector<std::string> values) :
		//variable  modifier                          value                felid
		Parent     (                                  values[index(fields, "id"         )]  ),
		name       (                                  values[index(fields, "name"       )]  ),
		icon       (                                  values[index(fields, "icon"       )]  ),
		owner      (getBool(                          values[index(fields, "owner"      )]) ),
		permissions(static_cast<Permission>(std::stoi(values[index(fields, "permissions")])))
	{}

	const std::initializer_list<const char*const> ServerUser::fields = {
		"id", "name", "icon", "owner", "permissions"
	};

	Connection::Connection(const std::string * rawJSON) : Connection(json::getValues(rawJSON->c_str(), fields)) {}

	Connection::Connection(const std::vector<std::string> values) :
		//variable modifier value            felid
		Parent (        values[index(fields, "id"     )] ),
		name   (        values[index(fields, "name"   )] ),
		type   (        values[index(fields, "type"   )] ),
		revoked(getBool(values[index(fields, "revoked")]))
	{}

	const std::initializer_list<const char*const> Connection::fields = {
		"id", "name", "type", "revoked"
	};
}