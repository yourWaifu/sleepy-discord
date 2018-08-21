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
		~User();
		User();
		User(const std::string * rawJSON);
		User(const std::vector<std::string> values);
		std::string username;
		std::string discriminator;
		std::string avatar;			//base64 encoded jpeg image
		//these are optional
		bool bot = false;
		bool mfa_enabled = false;	//true if two-factor authentication is enabled
		bool verified = false;		//true if email has been verified
		std::string email = "";
	private:
		const static std::initializer_list<const char*const> fields;
	};

	/*
	User Guild Structure    A brief version of a guild object

	Field	Type	Description
	id	snowflake	guild.id
	name	string	guild.name
	icon	string	guild.icon
	owner	bool	true if the user is an owner of the guild
	permissions	integer	bitwise of the user's enabled/disabled permissions
	*/
	struct ServerUser : public IdentifiableDiscordObject<ServerUser> {
	public:
		ServerUser();
		ServerUser(const std::string * rawJSON);
		ServerUser(const std::vector<std::string> values);
		std::string name;
		std::string icon;
		bool owner;
		Permission permissions;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	typedef ServerUser UserServer; //useful for intellisense

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
		Connection() {}
		Connection(const std::string * rawJSON);
		Connection(const std::vector<std::string> values);
		std::string name;
		std::string type;
		bool revoked;
	private:
		const static std::initializer_list<const char*const> fields;
	};
}