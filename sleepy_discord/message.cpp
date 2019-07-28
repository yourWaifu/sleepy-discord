#include "message.h"
#include "client.h"

namespace SleepyDiscord {
	Message::Message(const json::Value& json) :
		Message(json::fromJSON<Message>(json))
	{}

	Message::Message(const nonstd::string_view& json) :
		Message(json::fromJSON<Message>(json))
	{}

	bool Message::startsWith(const std::string& test) {
		return content.compare(0, test.length(), test) == 0;
	}

	std::size_t Message::length() {
		return content.length();
	}

	bool Message::isMentioned(Snowflake<User> userID) {
		std::size_t size = mentions.size();
		for (std::size_t i = 0; i < size; i++)
			if (mentions[i].ID == userID) return true;
		return false;
	}

	bool Message::isMentioned(User& _user) {
		return isMentioned(_user.ID);
	}

	Message Message::send(BaseDiscordClient* client) {
		return client->sendMessage(channelID, content, !embeds.empty() ? embeds[0] : Embed(), tts);
	}

	Message Message::reply(BaseDiscordClient * client, std::string message, Embed embed, bool _tts)
	{
		return client->sendMessage(channelID, message, embed, _tts);
	}

	Emoji::~Emoji() {
	}

	Emoji::Emoji(const json::Value& json) :
		Emoji(json::fromJSON<Emoji>(json)) {
	}

	Emoji::Emoji(const nonstd::string_view& json) :
		Emoji(json::fromJSON<Emoji>(json)) {
	}

	Reaction::~Reaction() {
	}

	Reaction::Reaction(const json::Value& json) :
		Reaction(json::fromJSON<Reaction>(json)) {
	}

	Reaction::Reaction(const nonstd::string_view& json) :
		Reaction(json::fromJSON<Reaction>(json)) {
	}
}