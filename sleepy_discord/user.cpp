#include "user.h"

namespace SleepyDiscord {
	//User::~User() {
	//}

	//User::User() {
	//}

	//const std::initializer_list<const char*const> User::fields = {
	//	"id", "username", "discriminator", "avatar", "bot", "mfa_enabled", "verified", "email"
	//};

	//User::User(const std::string * rawJSON) : User(json::getValues(rawJSON->c_str(), fields)) {}

	//User::User(const nonstd::string_view& rawJSON) : User(json::getValues(rawJSON.data(), fields)) {}

	//User::User(const json::Values values) :
	//	//variable     modifier value               felid
	//	Parent        (        values[index(fields, "id"           )] ),
	//	username      (        values[index(fields, "username"     )] ),
	//	discriminator (        values[index(fields, "discriminator")] ),
	//	avatar        (        values[index(fields, "avatar"       )] ),
	//	bot           (getBool(values[index(fields, "bot"          )])),
	//	mfa_enabled   (getBool(values[index(fields, "mfa_enabled"  )])),
	//	verified      (getBool(values[index(fields, "verified"     )])),
	//	email         (        values[index(fields, "email"        )] )
	//{}

	//ServerUser::ServerUser() : owner(false), permissions(NONE)
	//{}
	//
	//ServerUser::ServerUser(const std::string * rawJSON) : ServerUser(json::getValues(rawJSON->c_str(), fields)) {}

	//ServerUser::ServerUser(const nonstd::string_view& rawJSON) : ServerUser(json::getValues(rawJSON.data(), fields)) {}

	//ServerUser::ServerUser(const json::Values values) :
	//	//variable  modifier                          value                felid
	//	Parent     (                                  values[index(fields, "id"         )]              ),
	//	name       (                                  values[index(fields, "name"       )]              ),
	//	icon       (                                  values[index(fields, "icon"       )]              ),
	//	owner      (getBool(                          values[index(fields, "owner"      )]            ) ),
	//	permissions(static_cast<Permission>(std::stoi(values[index(fields, "permissions")].to_string())))
	//{}

	//const std::initializer_list<const char*const> ServerUser::fields = {
	//	"id", "name", "icon", "owner", "permissions"
	//};

	//Connection::Connection() : revoked(false)
	//{}

	//Connection::Connection(const std::string * rawJSON) : Connection(json::getValues(rawJSON->c_str(), fields)) {}

	//Connection::Connection(const nonstd::string_view& rawJSON) : Connection(json::getValues(rawJSON.data(), fields)) {}

	//Connection::Connection(const json::Values values) :
	//	//variable modifier value            felid
	//	Parent (        values[index(fields, "id"     )] ),
	//	name   (        values[index(fields, "name"   )] ),
	//	type   (        values[index(fields, "type"   )] ),
	//	revoked(getBool(values[index(fields, "revoked")]))
	//{}

	//const std::initializer_list<const char*const> Connection::fields = {
	//	"id", "name", "type", "revoked"
	//};

	User::User(const json::Value & json) :
		User(json::fromJSON<User>(json)) {
	}
	User::User(const nonstd::string_view & json) :
		User(json::fromJSON<User>(json)) {
	}

	Connection::Connection(const json::Value & json) :
		Connection(json::fromJSON<Connection>(json)) {
	}
	Connection::Connection(const nonstd::string_view & json) :
		Connection(json::fromJSON<Connection>(json)) {
	}
}