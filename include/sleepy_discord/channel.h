#pragma once
#include <string>
#include "user.h"
#include "snowflake.h"

namespace SleepyDiscord {
	struct Overwrite : DiscordObject {
		Overwrite() {}
		Overwrite(const std::string * rawJson);
		Overwrite(const std::vector<std::string> values);
		~Overwrite() {}
		Snowflake<Overwrite> ID;
		std::string type;
		int allow;
		int deny;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	//forward declearion
	struct Server;
	struct Message;

	class Channel : public DiscordObject {
	public:
		Channel() {}
		Channel(const std::string * rawJson);
		Channel(const std::vector<std::string> values);
		~Channel();
		Snowflake<Channel> ID;
		Snowflake<Server> serverID;
		std::string name;
		enum ChannelType {
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
		Snowflake<Message> lastMessageID;
		int bitrate;
		int userLimit;
		Snowflake<Message> parentID;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	struct DirectMessageChannel : public DiscordObject {
		DirectMessageChannel() {}
		DirectMessageChannel(const std::string * rawJson);
		DirectMessageChannel(const std::vector<std::string> values);
		~DirectMessageChannel();
		Snowflake<DirectMessageChannel> ID;
		bool is_private;
		User recipient;
		Snowflake<Message> lastMessageID;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	typedef DirectMessageChannel DMChannel;
}