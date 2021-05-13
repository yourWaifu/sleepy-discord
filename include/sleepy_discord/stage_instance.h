#pragma once
#include "discord_object_interface.h"
#include "server.h"

namespace SleepyDiscord {
	struct StageInstance : IdentifiableDiscordObject<StageInstance> {
		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		std::string topic;

		JSONStructStart
			std::make_tuple(
				json::pair                     (&StageInstance::ID       , "id"        , json::REQUIRIED_FIELD),
				json::pair                     (&StageInstance::serverID , "guild_id"  , json::REQUIRIED_FIELD),
				json::pair                     (&StageInstance::channelID, "channel_id", json::REQUIRIED_FIELD),
				json::pair                     (&StageInstance::topic    , "topic"     , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};
}