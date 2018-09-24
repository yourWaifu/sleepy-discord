#pragma once
#include <string>
#include "discord_object_interface.h"
#include "user.h"
#include "server.h"
#include "channel.h"
#include "snowflake.h"

namespace SleepyDiscord {
	/*
	Invite Structure	Represents a code that when used, adds a user to a guild.

	Field      Type                      Description
	code       string                    the invite code (unique ID)
	guild      a invite guild object     the guild this invite is for
	channel    a invite channel object   the channel this invite is for
	*/
	struct Invite : public DiscordObject {
	public:
		Invite() = default;
		//~Invite();
		//Invite(const std::string * rawJson);
		Invite(const json::Value & json);
		Invite(const nonstd::string_view & rawJSON);
		//Invite(const json::Values values);
		std::string code;
		Server server;
		Channel channel;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&Invite::code   , "code"   , json::REQUIRIED_FIELD),
				json::pair(&Invite::server , "guild"  , json::OPTIONAL_FIELD ),
				json::pair(&Invite::channel, "channel", json::REQUIRIED_FIELD)
			);
		JSONStructEnd
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
		InviteMetadata() = default;
		//~InviteMetadata();
		//InviteMetadata(const std::string * rawJson);
		//InviteMetadata(const json::Values values);
		InviteMetadata(const json::Value & json);
		InviteMetadata(const nonstd::string_view & json);
		User inviter;
		int uses = 0;
		int max_users = 0;
		int max_age = 0;
		bool isTemporary;
		std::string createAt;
		bool revoked;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&InviteMetadata::inviter    , "inviter"   , json::REQUIRIED_FIELD),
				json::pair(&InviteMetadata::uses       , "uses"      , json::REQUIRIED_FIELD),
				json::pair(&InviteMetadata::max_users  , "max_uses"  , json::REQUIRIED_FIELD),
				json::pair(&InviteMetadata::max_age    , "max_age"   , json::REQUIRIED_FIELD),
				json::pair(&InviteMetadata::isTemporary, "temporary" , json::REQUIRIED_FIELD),
				json::pair(&InviteMetadata::createAt   , "created_at", json::REQUIRIED_FIELD),
				json::pair(&InviteMetadata::revoked    , "revoked"   , json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};
}