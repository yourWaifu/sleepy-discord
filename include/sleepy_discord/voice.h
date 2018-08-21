#pragma once
#include "discord_object_interface.h"
#include "snowflake.h"

namespace SleepyDiscord {
	//forward declearion
	struct Server;
	struct Channel;
	struct User;

	struct VoiceState : public DiscordObject {
		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		Snowflake<User> userID;
		std::string sessionID;
		bool deaf;
		bool mute;
		bool selfDeaf;
		bool selfMute;
		bool suppress;
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
}