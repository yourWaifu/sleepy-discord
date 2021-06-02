#pragma once
#include "discord_object_interface.h"
#include "server.h"

namespace SleepyDiscord {
	struct StageInstance : IdentifiableDiscordObject<StageInstance> {
		StageInstance() = default;
		~StageInstance() = default;
		StageInstance(const json::Value & json);
		StageInstance(const nonstd::string_view & json);

		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		std::string topic;
		using PrivacyLevelRaw = int;
		enum class PrivacyLevel : PrivacyLevelRaw {
			NotSet = -1, //made up for the library
			PUBLIC = 1,
			SERVER_ONLY = 2
		};
		PrivacyLevel privacyLevel = PrivacyLevel::NotSet;
		bool discoverableDisabled = false;

		JSONStructStart
			std::make_tuple(
				json::pair                      (&StageInstance::ID                  , "id"                   , json::REQUIRIED_FIELD),
				json::pair                      (&StageInstance::serverID            , "guild_id"             , json::REQUIRIED_FIELD),
				json::pair                      (&StageInstance::channelID           , "channel_id"           , json::REQUIRIED_FIELD),
				json::pair                      (&StageInstance::topic               , "topic"                , json::OPTIONAL_FIELD ),
				json::pair<json::EnumTypeHelper>(&StageInstance::privacyLevel        , "privacy_level"        , json::OPTIONAL_FIELD ),
				json::pair                      (&StageInstance::discoverableDisabled, "discoverable_disabled", json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

	template<>
	struct GetDefault<StageInstance::PrivacyLevel> {
		static inline const StageInstance::PrivacyLevel get() {
			return StageInstance::PrivacyLevel::NotSet;
		}
	};
}