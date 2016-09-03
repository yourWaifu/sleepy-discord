#pragma once
#include <string>
#include "user.h"

namespace SleepyDiscord {
	class Channel : public DiscordObject {
	public:
		using DiscordObject::DiscordObject;
		~Channel();
		void fillOut(const char* name, void * value);
		std::string id;
		std::string guild_id;
		std::string name;
		std::string type;
		double* position;
		bool* is_private;
		//Overwrite* permissionOverwrites;
		std::string topic;
		std::string lastMessage_id;
		double* bitrate;
		double* userLimit;
	};

	class DirectMessageChannel : public DiscordObject {
		using DiscordObject::DiscordObject;
		~DirectMessageChannel();
		void fillOut(const char* name, void * value);
		std::string id;
		bool* is_private;
		User recipient;
		std::string lastMessage_id;
	};

	typedef DirectMessageChannel DMChannel;
}