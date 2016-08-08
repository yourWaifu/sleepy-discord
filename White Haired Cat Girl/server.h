#pragma once
#include <string>
#include "client.h"
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct Server : public DiscordObject {
		Server(JSON* jsonMessage);
		~Server();
		void fillOut(JSON_object* _JSON_object);
		void fillOut(const char* name, void * value);
		std::string id;
		std::string name;
		std::string icon;
		std::string splash;
		std::string owner_id;
		std::string region;
		std::string afk_channel_id;
		double* afk_timeout;
		bool* embed_enable;
		std::string embed_channel_id;
		double* verfication_level;
		//voice_states
		//roles
		//emojis
		//features
		bool* unavailable;

		//presences
		unsigned int numOfPressences;
		double* mfa_level;
		//members
		unsigned int numOfMembers;
		bool* large;
		std::string joined_at;
		double* default_message_notifications;
	};
}