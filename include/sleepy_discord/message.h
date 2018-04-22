#pragma once
#include "user.h"
#include "attachment.h"
#include "embed.h"
#include "permissions.h"
#include "webhook.h"
#include "discord_object_interface.h"
#include "snowflake.h"
#include "channel.h"

// <--- means to add later

namespace SleepyDiscord {
	struct Emoji : public DiscordObject {
	public:
		~Emoji();
		Emoji();
		Emoji(const std::string* rawJson);
		Emoji(const std::vector<std::string> values);
		Snowflake<Emoji> ID;
		std::string name;
		std::vector<Role> roles;
		User user;	//optional
		bool requireColons;
		bool managed;
	private:
		const static std::initializer_list<const char*const> fields;
	};
	
	struct Reaction : public DiscordObject {
	public:
		Reaction();
		~Reaction();
		Reaction(const std::string * rawJson);
		Reaction(const std::vector<std::string> values);
		int count;
		bool me;
		Emoji emoji;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	class BaseDiscordClient;

	struct Message : public DiscordObject {
	public:
		Message();
		~Message();
		Message(const std::vector<std::string> values);
		Message(const std::string * rawJson);
		using DiscordObject::DiscordObject;
		bool startsWith(const std::string& test);
		int length();
		bool isMentioned(Snowflake<User> ID);
		bool isMentioned(User& _user);
		Message send(BaseDiscordClient * client);
		Message reply(BaseDiscordClient * client, std::string message, bool tts = false);
		//bool operator==(const std::string& rawJson);
		bool operator==(const Message& right) const;
		bool operator!=(const Message& right) const;

		Snowflake<Message> ID;
		Snowflake<Channel> channelID;
		User author;
		std::string content;
		std::string timestamp;
		std::string edited_timestamp;
		bool tts;
		bool mention_everyone;
		std::vector<User> mentions;
		std::vector<Snowflake<User>> mention_roles;
		std::vector<Attachment> attachments;
		std::vector<Embed> embeds;
		std::vector<Reaction> reactions;
		int64_t nonce;	//nullable
		bool pinned;
		Snowflake<Webhook> webhookID;
		enum MessageType {
			DEFAULT                = 0,
			RECIPIENT_ADD          = 1,
			RECIPIENT_REMOVE       = 2,
			CALL                   = 3,
			CHANNEL_NAME_CHANGE    = 4,
			CHANNEL_ICON_CHANGE    = 5,
			CHANNEL_PINNED_MESSAGE = 6,
			GUILD_MEMBER_JOIN      = 7
		} type;
	private:
		const static std::initializer_list<const char*const> fields;
	};
}