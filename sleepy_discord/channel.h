#pragma once
#include <string>
#include "user.h"

namespace SleepyDiscord {
	class Channel : public DiscordObject {
	public:
		using DiscordObject::DiscordObject;
		Channel(const std::string * rawJson);
		~Channel();
		std::string id;
		std::string guild_id;
		std::string name;
		std::string type;
		int position;
		bool isPrivate;
		//Overwrite* permissionOverwrites;
		std::string topic;
		std::string lastMessage_id;
		int bitrate;
		int userLimit;
	};

	class DirectMessageChannel : public DiscordObject {
		using DiscordObject::DiscordObject;
		~DirectMessageChannel();
		std::string id;
		bool* is_private;
		User recipient;
		std::string lastMessage_id;
	};

	typedef DirectMessageChannel DMChannel;
}