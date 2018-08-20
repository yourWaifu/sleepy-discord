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
		VoiceState();
		VoiceState(const std::string * rawJson);
		VoiceState(const std::vector<std::string> values);
		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		Snowflake<User> userID;
		std::string sessionID;
		bool deaf;
		bool mute;
		bool selfDeaf;
		bool selfMute;
		bool suppress;
	private:
		const static std::initializer_list<const char*const> fields;
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
		VoiceRegion();
		VoiceRegion(const std::string * rawJson);
		VoiceRegion(const std::vector<std::string> values);
		std::string name;
		std::string sampleHostname;
		int samplePort;
		bool vip;
		bool optimal;
		bool deprecated;
		bool custom;

	private:
		const static std::initializer_list<const char*const> fields;
	};

	/*
	Voice Server Update Event Fields
	Field     Type       Description
	token     string     voice connection token
	guild_id  snowflake  the guild this voice server update is for
	endpoint  string     the voice server host
	*/
	struct VoiceServerUpdate : DiscordObject {
		VoiceServerUpdate();
		VoiceServerUpdate(const std::string * rawJson);
		VoiceServerUpdate(const std::vector<std::string> values);
		std::string token;
		Snowflake<Server> serverID;
		std::string endpoint;
	private:
		const static std::initializer_list<const char*const> fields;
	};
}