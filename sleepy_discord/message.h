#pragma once
#include "user.h"
#include "attachment.h"
#include "embed.h"
#include "discord_object_interface.h"

// <--- means to add later

namespace SleepyDiscord {
	class BaseDiscordClient;

	struct Message : public DiscordObject {
	public:
		Message();
		~Message();
		Message(const std::string * rawJson);
		Message(BaseDiscordClient* client, std::string channel_id, std::string message, bool tts = false);
		using DiscordObject::DiscordObject;
		bool startsWith(char* test);
		int length();
		bool isMentioned(const std::string id);
		bool isMentioned(User& _user);
		Message send(BaseDiscordClient * client);
		Message reply(BaseDiscordClient * client, std::string message, bool tts = false);
		std::string id;
		std::string channel_id;
		User author;
		std::string content;
		std::string timestamp;
		std::string edited_timestamp;
		bool tts;
		bool mention_everyone;
		std::vector<User> mentions;
		std::vector<Attachment> attachments;
		std::vector<Embed> embeds;
		std::vector<std::string> mention_roles;
		int64_t nonce;	//nullable
		bool pinned;

		double type;
	};
}