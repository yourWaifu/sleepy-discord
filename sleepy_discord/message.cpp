#include "message.h"
#include "client.h"

namespace SleepyDiscord {
	Message::Message(json::Value& json) :
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
		return client->sendMessage(channelID, content, !embeds.empty() ? embeds[0] : Embed(), messageReference, static_cast<TTS>(tts));
	}

	Message Message::reply(BaseDiscordClient * client, std::string message, Embed embed)
	{
		return client->sendMessage(channelID, message, embed);
	}

	Message::Interaction::Interaction(const json::Value& json) :
		Interaction(json::fromJSON<Message::Interaction>(json))
	{}

	Emoji::~Emoji() {
	}

	Emoji::Emoji(const json::Value& json) :
		Emoji(json::fromJSON<Emoji>(json)) {
	}

	Reaction::~Reaction() {
	}

	Reaction::Reaction(const json::Value& json) :
		Reaction(json::fromJSON<Reaction>(json)) {
	}

	StickerPack::~StickerPack() {}
	StickerPack::StickerPack(const json::Value & json):
		StickerPack(json::fromJSON<StickerPack>(json))
	{}

	Sticker::~Sticker() {}
	Sticker::Sticker(const json::Value & json):
		Sticker(json::fromJSON<Sticker>(json))
	{}

	MessageReference::MessageReference(const json::Value & json):
		MessageReference(json::fromJSON<MessageReference>(json))
	{}
	AllowedMentions::AllowedMentions(const json::Value & json):
		AllowedMentions(json::fromJSON<AllowedMentions>(json))
	{}

	ActionRow::ActionRow(json::Value& json) :
		ActionRow(json::fromJSON<ActionRow>(json))
	{}
	Button::Button(const json::Value& json) :
		Button(json::fromJSON<Button>(json))
	{}
	SelectMenu::SelectMenu(const json::Value& json) :
		SelectMenu(json::fromJSON<SelectMenu>(json))
	{}
	SelectMenu::Option::Option(const json::Value& json) :
		SelectMenu::Option(json::fromJSON<SelectMenu::Option>(json))
	{}
	TextInputs::TextInputs(const json::Value& json) :
		TextInputs(json::fromJSON<TextInputs>(json))
	{}

}