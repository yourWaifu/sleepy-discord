#pragma once
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "user.h"
#include "attachment.h"
#include "embed.h"
#include "discord_object_interface.h"

// <--- means to add later

namespace SleepyDiscord {
	struct Message : public DiscordObject {
	public:
		Message();
		~Message();
		Message(JSON_object * messageJSONObject);
		Message(JSON * jsonMessage);
		Message(const std::string * rawJson, const unsigned int startPosition = 0);
		using DiscordObject::DiscordObject;
		//void fillOut(JSON_object * _JSON_object);
		void fillOut(const char* name, void * value);
		bool startsWith(char* test);
		int length();
		bool isMentioned(const std::string id);
		bool isMentioned(User& _user);
		//void reply(std::string message);
		std::string id;
		std::string channel_id;
		User author;
		std::string content;
		std::string timestamp;
		std::string edited_timestamp;
		bool tts;
		bool mention_everyone;
		User* mentions;
		unsigned int numOfMentions;
		Attachment* attachments;
		unsigned int numOfAttachments;
		Embed* embeds;
		unsigned int numOfEmbeds;
		std::string* mention_roles;
		unsigned int numOfMention_roles;
		int64_t nonce;	//nullable
		bool pinned;

		double type;
	};
}