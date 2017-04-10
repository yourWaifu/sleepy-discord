#include "message.h"
#include "client.h"

namespace SleepyDiscord {
	Message::Message() {

	}

	Message::~Message() {
	}

	Message::Message(const std::string * rawJson) 
	{
		//set default values
		nonce = 0;
		//parse json and convert from string to type
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
			{ "id", "channel_id", "content", "timestamp" , "edited_timestamp", "tts",
		      "mention_everyone", "author", "mentions", "mention_roles", "attachments",
		      "embeds", "nonce", "pinned" });
		id = values[0];
		channel_id = values[1];
		content = values[2];
		timestamp = values[3];
		edited_timestamp = values[4];
		tts = values[5][0] == 't';
		mention_everyone = values[6][0] == 't';
		author = User(&values[7]);
		//these if statements are here because they make the library faster
		JSON_getArray<User>(&values[8], &mentions);
		json::getArray(&values[9], &mention_roles);
		JSON_getArray<Attachment>(&values[10], &attachments);
		JSON_getArray<Embed>(&values[11], &embeds);
		if (values[12][0] && values[12][0] != 'n') nonce = std::stoll(values[12]);
		pinned = values[13][0] == 't';
	}

	Message::Message(BaseDiscordClient* client, std::string channel_id, std::string message, bool tts)
	{
		*this = client->sendMessage(channel_id, message, tts);
	}

	bool Message::startsWith(char* test) {
		return strncmp(content.c_str(), test, strlen(test)) == 0;
	}

	int Message::length() {
		return content.length();	//if this doesn't work there's a stringLength function in common.h
	}

	bool Message::isMentioned(const std::string id) {
		unsigned int size = mentions.size();
		for (unsigned int i = 0; i < size; i++)
			if (mentions[i].id  == id) return true;
		return false;
	}

	bool Message::isMentioned(User& _user) {
		return isMentioned(_user.id);
	}

	Message Message::send(BaseDiscordClient* client) {
		return client->sendMessage(channel_id, content, tts);
	}

	Message Message::reply(BaseDiscordClient * client, std::string message, bool tts)
	{
		return client->sendMessage(channel_id, message, tts);
	}
}