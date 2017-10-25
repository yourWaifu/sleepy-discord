#include "message.h"
#include "client.h"

namespace SleepyDiscord {
	Message::Message() {

	}

	Message::~Message() {
	}

	Message::Message(const std::string * rawJson) {
				//parse json and convert from string to type
		std::initializer_list<const char*const> names = {
			"id", "channel_id", "content", "timestamp" , "edited_timestamp", "tts",
			"mention_everyone", "author", "mentions", "mention_roles", "attachments",
			"embeds", "nonce", "pinned"
		};
		std::vector<std::string> values = json::getValues(rawJson->c_str(), names);
		if (values[0] == "")//error check
			return;

		id               =                            values[index(names, "id"              )] ;
		channel_id       =                            values[index(names, "channel_id"      )] ;
		content          =                            values[index(names, "content"         )] ;
		timestamp        =                            values[index(names, "timestamp"       )] ;
		edited_timestamp =                            values[index(names, "edited_timestamp")] ;
		tts              = getBool(                   values[index(names, "tts"             )]);
		mention_everyone = getBool(                   values[index(names, "mention_everyone")]);
		author           = User(                     &values[index(names, "author"          )]);
		mentions         = JSON_getArray<User>(      &values[index(names, "mentions"        )]);
		mention_roles    = json::getArray(           &values[index(names, "mention_roles"   )]);
		attachments      = JSON_getArray<Attachment>(&values[index(names, "attachments"     )]);
		embeds           = JSON_getArray<Embed>(     &values[index(names, "embeds"          )]);
		modIfElse(isDefined, nonce, 0, values[index(names, "nonce")]);
		pinned           = getBool(                   values[index(names, "pinned"          )]);
	}

	Message::Message(BaseDiscordClient* client, std::string channel_id, std::string message, bool tts)
	{
		*this = client->sendMessage(channel_id, message, tts);
	}

	bool Message::startsWith(char* test) {
		return strncmp(content.c_str(), test, strlen(test)) == 0;
	}

	int Message::length() {
		return content.length();
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

	bool Message::operator==(const Message& right) {
		return id == right.id;
	}

	SleepyDiscord::ReactionEmoji::ReactionEmoji() {
	}

	SleepyDiscord::ReactionEmoji::ReactionEmoji(const std::string * rawJson) {

	}

	SleepyDiscord::Reaction::Reaction() {
	}

	SleepyDiscord::Reaction::~Reaction() {
	}

	Reaction::Reaction(const std::string * rawJson) {
		std::initializer_list<const char*const> names = {
			"count", "me", "emoji"
		};

		std::vector<std::string> values = json::getValues(rawJson->c_str(), names);
		count = std::stoul(values[index(names, "count")]);
		me    = getBool(   values[index(names, "me"   )]);
		//emoji = Emoji(&values[index(names, "emoji"   )]);
	}
}