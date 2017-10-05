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
		enum ChannelType{
			SERVER_TEXT     = 0,
			DM              = 1,
			SERVER_VOICE    = 2,
			GROUP_DM        = 3,
			SERVER_CATEGORY = 4
		} type;
		int position;
		bool isPrivate;
		Overwrite permissionOverwrites;
		std::string topic;
		std::string lastMessage_id;
		int bitrate;
		int userLimit;
		std::string parent_id;
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