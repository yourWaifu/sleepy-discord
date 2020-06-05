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
		Webhook() = default;
		Webhook(const json::Value & json);
		Webhook(const nonstd::string_view & rawJSON);

		enum WebhookType {
			INCOMING = 1,
			CHANNEL_FOLLOWER = 2,
		};
		WebhookType type = static_cast<WebhookType>(0);
		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		User user;
		std::string name;
		std::string avatar;
		std::string token;
		JSONStructStart
			std::make_tuple(
				json::pair(&Webhook::ID       , "id"        , json::REQUIRIED_FIELD),
				json::pair(&Webhook::serverID , "guild_id"  , json::OPTIONAL_FIELD ),
				json::pair(&Webhook::channelID, "channel_id", json::REQUIRIED_FIELD),
				json::pair(&Webhook::user     , "user"      , json::OPTIONAL_FIELD ),
				json::pair(&Webhook::name     , "name"      , json::NULLABLE_FIELD ),
				json::pair(&Webhook::avatar   , "avatar"    , json::NULLABLE_FIELD ),
				json::pair(&Webhook::token    , "token"     , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};
}