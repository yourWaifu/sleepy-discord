#pragma once
#include "discord_object_interface.h"
#include "snowflake.h"
#include "channel.h"

namespace SleepyDiscord {
	//forward declearion
	struct Server;
	struct Channel;
	struct User;

	struct VoiceState : public DiscordObject {
		VoiceState() = default;
		VoiceState(const nonstd::string_view & rawJSON);
		VoiceState(const json::Value& json);
		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		Snowflake<User> userID;
		std::string sessionID;
		bool deaf = false;
		bool mute = false;
		bool selfDeaf = false;
		bool selfMute = false;
		bool suppress = false;
		JSONStructStart
			std::make_tuple(
				json::pair(&VoiceState::serverID , "guild_id"  , json::OPTIONAL_FIELD ),
				json::pair(&VoiceState::channelID, "channel_id", json::NULLABLE_FIELD ),
				json::pair(&VoiceState::userID   , "user_id"   , json::REQUIRIED_FIELD),
				json::pair(&VoiceState::sessionID, "session_id", json::REQUIRIED_FIELD),
				json::pair(&VoiceState::deaf     , "deaf"      , json::REQUIRIED_FIELD),
				json::pair(&VoiceState::mute     , "mute"      , json::REQUIRIED_FIELD),
				json::pair(&VoiceState::selfDeaf , "self_deaf" , json::REQUIRIED_FIELD),
				json::pair(&VoiceState::selfMute , "self_mute" , json::REQUIRIED_FIELD),
				json::pair(&VoiceState::suppress , "suppress"  , json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};

	/*
	Voice Region Structure
	Field           Type    Description
	id              string  unique ID for the region
	name            string  name of the region
	sample_hostname string  an example hostname for the region
	sample_port     integer an example port for the region
	vip             bool    true if this is a vip-only server
	optimal         bool    true for a single server that is closest to the current user's client
	deprecated      bool    whether this is a deprecated voice region (avoid switching to these)
	custom          bool    whether this is a custom voice region (used for events/etc)
	*/
	struct VoiceRegion : IdentifiableDiscordObject<VoiceRegion> {
		VoiceRegion() = default;
		//VoiceRegion(const std::string * rawJson);
		VoiceRegion(const nonstd::string_view & rawJSON);
		VoiceRegion(const json::Value& json);
		//VoiceRegion(const json::Values values);
		std::string name;
		bool vip = false;
		bool optimal = false;
		bool deprecated = false;
		bool custom = false;
		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&VoiceRegion::ID            , "id"        , json::REQUIRIED_FIELD),
				json::pair(&VoiceRegion::name          , "name"      , json::REQUIRIED_FIELD),
				json::pair(&VoiceRegion::vip           , "vip"       , json::REQUIRIED_FIELD),
				json::pair(&VoiceRegion::optimal       , "optimal"   , json::REQUIRIED_FIELD),
				json::pair(&VoiceRegion::deprecated    , "deprecated", json::REQUIRIED_FIELD),
				json::pair(&VoiceRegion::custom        , "custom"    , json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};

	/*
	Voice Server Update Event Fields
	Field     Type       Description
	token     string     voice connection token
	guild_id  snowflake  the guild this voice server update is for
	endpoint  string     the voice server host
	*/
	struct VoiceServerUpdate : DiscordObject {
		VoiceServerUpdate() = default;
		//VoiceServerUpdate(const std::string * rawJson);
		VoiceServerUpdate(const nonstd::string_view & rawJSON);
		VoiceServerUpdate(const json::Value& json);
		//VoiceServerUpdate(const json::Values values);
		std::string token;
		Snowflake<Server> serverID;
		std::string endpoint;
		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&VoiceServerUpdate::token   , "token"   , json::REQUIRIED_FIELD),
				json::pair(&VoiceServerUpdate::serverID, "guild_id", json::REQUIRIED_FIELD),
				json::pair(&VoiceServerUpdate::endpoint, "endpoint", json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};
}