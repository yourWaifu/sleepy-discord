#pragma once
#include <string>
#include "discord_object_interface.h"
#include "user.h"
#include "snowflake.h"

namespace SleepyDiscord {
	/*
	Invite Channel Structure	Represents the channel an invite is for.

	Field	Type		Description
	id      snowflake	id of the channel
	name    string      name of the channel
	type    string      'text' or 'voice'
	*/
	struct ChannelInvite : IdentifiableDiscordObject<ChannelInvite> {
	public:
		ChannelInvite();
		~ChannelInvite();
		ChannelInvite(const std::string * rawJson);
		ChannelInvite(const std::vector<std::string> values);
		std::string name;
		std::string type;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	/*
	Invite Guild Structure	Represents the parent guild of an invite

	Field   Type        Description
	id      snowflake   id of the guild
	name    string      name of the guild
	splash  string      hash of the guild splash (or null)
	icon    string      hash of the guild icon (or null)
	*/
	struct ServerInvite : IdentifiableDiscordObject<ServerInvite> {
	public:
		ServerInvite();
		~ServerInvite();
		ServerInvite(const std::string * rawJson);
		ServerInvite(const std::vector<std::string> values);
		std::string name;
		std::string splash;
		std::string icon;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	/*
	Invite Structure	Represents a code that when used, adds a user to a guild.

	Field      Type                      Description
	code       string                    the invite code (unique ID)
	guild      a invite guild object     the guild this invite is for
	channel    a invite channel object   the channel this invite is for
	*/
	struct Invite : public DiscordObject {
	public:
		Invite();
		~Invite();
		Invite(const std::string * rawJson);
		Invite(const std::vector<std::string> values);
		std::string code;
		ServerInvite server;
		ChannelInvite channel;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	/*
	Invite Metadata Structure

	Field       Type            Description
	inviter     a user object   user who created the invite
	uses        integer         number of times this invite has been used
	max_uses    integer         max number of times this invite can be used
	max_age     integer         duration (in seconds) after which the invite expires
	temporary   bool            whether this invite only grants temporary membership
	created_at  datetime        when this invite was created
	revoked     bool            whether this invite is revoked
	*/
	struct InviteMetadata : public DiscordObject {
		InviteMetadata();
		~InviteMetadata();
		InviteMetadata(const std::string * rawJson);
		InviteMetadata(const std::vector<std::string> values);
		User inviter;
		int uses;
		int max_users;
		int max_age;
		bool isTemporary;
		std::string createAt;	//you may want to use a different type for this one
		bool isRevoked;
	private:
		const static std::initializer_list<const char*const> fields;
	};
}