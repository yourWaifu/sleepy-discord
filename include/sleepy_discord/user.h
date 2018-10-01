#pragma once
#include <string>
#include "discord_object_interface.h"
#include "snowflake.h"
#include "permissions.h"

namespace SleepyDiscord {
	/*
	User Structure

	Field           Type        Description                                                 Required OAuth2 Scope
	id              snowflake   the user's id                                               identify
	username        string      the user's username, not unique across the platform         identify
	discriminator   string      the user's 4-digit discord-tag                              identify
	avatar          string      the user's avatar hash                                      identify
	bot             bool        whether the user belongs to an OAuth2 application           identify
	mfa_enabled     bool        whether the user has two factor enabled on their account	identify
	verified        bool        whether the email on this account has been verified	        email
	email           string      the user's email                                            email
	*/
	struct User : public IdentifiableDiscordObject<User> {
	public:
		User() = default;
		//~User();
		//User(const std::string * rawJSON);
		User(const nonstd::string_view & json);
		User(const json::Value& json);
		//User(const json::Values values);
		std::string username;
		std::string discriminator;
		std::string avatar;			//base64 encoded jpeg image
		//these are optional
		bool bot = false;
		bool mfa_enabled = false;	//true if two-factor authentication is enabled
		bool verified = false;		//true if email has been verified
		std::string email = "";

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&User::ID           , "id"           , json::REQUIRIED_FIELD),
				json::pair(&User::username     , "username"     , json::REQUIRIED_FIELD),
				json::pair(&User::discriminator, "discriminator", json::REQUIRIED_FIELD),
				json::pair(&User::avatar       , "avatar"       , json::NULLABLE_FIELD ),
				json::pair(&User::bot          , "bot"          , json::OPTIONAL_FIELD ),
				json::pair(&User::mfa_enabled  , "mfa_enabled"  , json::OPTIONAL_FIELD ),
				json::pair(&User::verified     , "verified"     , json::OPTIONAL_FIELD ),
				json::pair(&User::email        , "email"        , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

	/*Connection Structure  The connection object that the user has attached.

	Field           Type    Description
	id              string  id of the connection account
	name            string  the username of the connection account
	type            string  the service of the connection (twitch, youtube)
	revoked         bool    whether the connection is revoked
	integrations	array   an array of partial server integrations
	*/
	struct Connection : public IdentifiableDiscordObject<Connection> {
	public:
		Connection() = default;
		//Connection(const std::string * rawJSON);
		Connection(const nonstd::string_view & json);
		Connection(const json::Value& json);
		//Connection(const json::Values values);
		std::string name;
		std::string type;
		bool revoked;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&Connection::ID     , "id"     , json::REQUIRIED_FIELD),
				json::pair(&Connection::name   , "name"   , json::REQUIRIED_FIELD),
				json::pair(&Connection::type   , "type"   , json::REQUIRIED_FIELD),
				json::pair(&Connection::revoked, "revoked", json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};
}