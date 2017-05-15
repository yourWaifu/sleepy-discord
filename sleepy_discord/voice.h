#pragma once
#include "discord_object_interface.h"

namespace SleepyDiscord {

	struct VoiceState : public DiscordObject {
		std::string guild_id;
		std::string channel_id;
		std::string user_id;
		std::string session_id;
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
	struct VoiceRegion : DiscordObject {
		VoiceRegion() {}
		VoiceRegion(const std::string * rawJson);
		std::string id;
		std::string name;
		std::string sample_hostname;
		int sample_port;
		bool vip;
		bool optimal;
		bool deprecated;
		bool custom;
	};
}