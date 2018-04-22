#include "message.h"
#include "client.h"

namespace SleepyDiscord {
	const std::initializer_list<const char*const> Message::fields = {
		"id", "channel_id", "content", "timestamp", "edited_timestamp", "tts",
		"mention_everyone", "author", "mentions", "mention_roles", "attachments",
		"embeds", "reactions", "nonce", "pinned", "webhook_id", "type"
	};

	Message::Message() {

	}

	Message::~Message() {
	}

	Message::Message(std::vector<std::string> values) : 
		//variable                  condition    modifier                         value                felid               else
		ID               (                                                        values[index(fields, "id"              )]                          ),
		channelID        (                                                        values[index(fields, "channel_id"      )]                          ),
		author           (                                                       &values[index(fields, "author"          )]                          ),
		content          (                                                        values[index(fields, "content"         )]                          ),
		timestamp        (                                                        values[index(fields, "timestamp"       )]                          ),
		edited_timestamp (                                                        values[index(fields, "edited_timestamp")]                          ),
		tts              (                       getBool                        ( values[index(fields, "tts"             )]                         )),
		mention_everyone (                       getBool                        ( values[index(fields, "mention_everyone")]                         )),
		mentions         (                       JSON_getArray<User>            (&values[index(fields, "mentions"        )]                         )),
		mention_roles    (                       JSON_getArray<Snowflake<User>> ( values[index(fields, "mention_roles"   )]                         )),
		attachments      (                       JSON_getArray<Attachment>      (&values[index(fields, "attachments"     )]                         )),
		embeds           (                       JSON_getArray<Embed>           (&values[index(fields, "embeds"          )]                         )),
		reactions        (modIfElse(isSpecified, JSON_getArray<Reaction>       , &values[index(fields, "reactions"       )], std::vector<Reaction>())),
		nonce            (modIfElse(isDefined  , toLongLong                    ,  values[index(fields, "nonce"           )], 0                      )),
		pinned           (                       getBool                        ( values[index(fields, "pinned"          )]                         )),
		webhookID        (setIfElse(isSpecified,                                  values[index(fields, "webhook_id"      )], ""                     )),
		type             (                       static_cast<MessageType>(toInt ( values[index(fields, "type"            )]                        )))
	{}

	Message::Message(const std::string * rawJson) : Message(json::getValues(rawJson->c_str(), fields)) {}

	bool Message::startsWith(const std::string& test) {
		return content.compare(0, test.length(), test) == 0;
	}

	int Message::length() {
		return content.length();
	}

	bool Message::isMentioned(Snowflake<User> ID) {
		unsigned int size = mentions.size();
		for (unsigned int i = 0; i < size; i++)
			if (mentions[i].ID == ID) return true;
		return false;
	}

	bool Message::isMentioned(User& _user) {
		return isMentioned(_user.ID);
	}

	Message Message::send(BaseDiscordClient* client) {
		return client->sendMessage(channelID, content, tts);
	}

	Message Message::reply(BaseDiscordClient * client, std::string message, bool tts)
	{
		return client->sendMessage(channelID, message, tts);
	}

	bool Message::operator==(const Message& right) const {
		return ID == right.ID;
	}

	bool Message::operator!=(const Message & right) const {
		return ID != right.ID;
	}

	const std::initializer_list<const char*const> Emoji::fields = {
		"id", "name", "roles", "user", "require_colons", "managed"
	};

	SleepyDiscord::Emoji::Emoji() {
	}

	SleepyDiscord::Emoji::~Emoji() {
	}

	Emoji::Emoji(const std::vector<std::string> values) :
		//variable     modifier             value                     felid
		ID           (                     values[index(fields, "id"            )] ),
		name          (                     values[index(fields, "name"          )] ),
		roles         (JSON_getArray<Role>(&values[index(fields, "roles"         )])),
		user          (                    &values[index(fields, "user"          )] ),
		requireColons (getBool(             values[index(fields, "require_colons")])),
		managed       (getBool(             values[index(fields, "managed"       )]))
	{}

	SleepyDiscord::Emoji::Emoji(const std::string * rawJson) : Emoji(json::getValues(rawJson->c_str(), fields)) {}

	const std::initializer_list<const char*const> Reaction::fields {
		"count", "me", "emoji"
	};

	SleepyDiscord::Reaction::Reaction() {
	}

	SleepyDiscord::Reaction::~Reaction() {
	}

	Reaction::Reaction(const std::vector<std::string> values) :
		//variable modifier value                 felid
		count(std::stoi(values[index(fields, "count")])),
		me   (getBool  (values[index(fields, "me"   )])),
		emoji(         &values[index(fields, "emoji")] )
	{}

	Reaction::Reaction(const std::string * rawJson) : Reaction(json::getValues(rawJson->c_str(), fields)) {}
}