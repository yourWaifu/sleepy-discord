#pragma once
#include <string>
#include "user.h"

namespace SleepyDiscord {
	struct Overwrite : DiscordObject {
		Overwrite() {}
		Overwrite(const std::string * rawJson);
		~Overwrite() {}
		std::string id;
		std::string type;
		int allow;
		int deny;
	};

	class Channel : public DiscordObject {
	public:
		Channel() {}
		Channel(const std::string * rawJson);
		~Channel();
		std::string id;
		std::string guild_id;
		std::string name;
		std::string type;
		int position;
		bool isPrivate;
		Overwrite permissionOverwrites;
		std::string topic;
		std::string lastMessage_id;
		int bitrate;
		int userLimit;
	};

	struct DirectMessageChannel : public DiscordObject {
		DirectMessageChannel() {}
		DirectMessageChannel(const std::string * rawJson);
		~DirectMessageChannel();
		std::string id;
		bool is_private;
		User recipient;
		std::string lastMessage_id;
	};

	typedef DirectMessageChannel DMChannel;
}