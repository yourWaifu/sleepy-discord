#pragma once
#include "discord_object_interface.h"
#include "user.h"
#include "server.h"
#include "channel.h"
#include "snowflake.h"

namespace SleepyDiscord {
	/*Webhook Structure
	Field       Type        Description
	id          snowflake   the id of the webhook
	guild_id    snowflake?  the guild id this webhook is for
	channel_id  snowflake   the channel id this webhook is for
	user        User?       the user this webhook was created by (not returned when getting a webhook with its token)
	name        ?string     the default name of the webhook
	avatar      ?string     the default avatar of the webhook
	token       string      the secure token of the webhook
	*/
	struct Webhook : public IdentifiableDiscordObject<Webhook> {
	public:
		Webhook() {}
		Webhook(const std::string * rawJSON);
		Webhook(const std::vector<std::string> values);
		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		User user;
		std::string name;
		std::string avatar;
		std::string token;
	private:
		const static std::initializer_list<const char*const> fields;
	};
}