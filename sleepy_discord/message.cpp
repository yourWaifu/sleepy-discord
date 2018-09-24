#include "message.h"
#include "client.h"

namespace SleepyDiscord {
	//const std::initializer_list<const char*const> Message::fields = {
	//	"id", "channel_id", "content", "timestamp", "edited_timestamp", "tts",
	//	"mention_everyone", "author", "mentions", "mention_roles", "attachments",
	//	"embeds", "reactions", "nonce", "pinned", "webhook_id", "type"
	//};

	//Message::Message() : tts(false), mentionEveryone(false), nonce(0), pinned(false), type(DEFAULT) {

	//}

	Message::~Message() {
	}

	Message::Message(const json::Value& json) :
		Message(json::fromJSON<Message>(json))
	{}

	Message::Message(const nonstd::string_view& json) :
		Message(json::fromJSON<Message>(json))
	{}

	//Message::Message(const json::Values values) :
	//	//variable                  condition    modifier                        value                felid               else
	//	Parent           (                                                       values["id               ]                          ),
	//	channelID        (                                                       values["channel_id"      ]                          ),
	//	author           (                                                       values["author"          ]                          ),
	//	content          (                                                       values["content"         ]                          ),
	//	timestamp        (                                                       values["timestamp"       ]                          ),
	//	editedTimestamp  (                                                       values["edited_timestamp"]                          ),
	//	tts              (                       getBool                        (values["tts"             ]                         )),
	//	mentionEveryone  (                       getBool                        (values["mention_everyone"]                         )),
	//	mentions         (                       JSON_getArray<User>            (values["mentions"        ]                         )),
	//	mentionRoles     (                       JSON_getArray<Snowflake<User>> (values["mention_roles"   ]                         )),
	//	attachments      (                       JSON_getArray<Attachment>      (values["attachments"     ]                         )),
	//	embeds           (                       JSON_getArray<Embed>           (values["embeds"          ]                         )),
	//	reactions        (modIfElse(isSpecified, JSON_getArray<Reaction>       , values["reactions"       ], std::vector<Reaction>())),
	//	nonce            (modIfElse(isDefined  , toLongLong                    , values["nonce"           ], 0                      )),
	//	pinned           (                       getBool                        (values["pinned"          ]                         )),
	//	webhookID        (setIfElse(isSpecified,                                 values["webhook_id"      ], ""                     )),
	//	type             (                       static_cast<MessageType>(toInt (values["type"            ]                        )))
	//{}

	//Message::Message(const std::string * rawJson) : Message(json::getValues(rawJson->c_str(), fields)) {}

	//Message::Message(const json::Value& rawJSON) : Message(json::getValues(rawJSON.data(), fields)) {}

	bool Message::startsWith(const std::string& test) {
		return content.compare(0, test.length(), test) == 0;
	}

	std::size_t Message::length() {
		return content.length();
	}

	bool Message::isMentioned(Snowflake<User> ID) {
		std::size_t size = mentions.size();
		for (std::size_t i = 0; i < size; i++)
			if (mentions[i].ID == ID) return true;
		return false;
	}

	bool Message::isMentioned(User& _user) {
		return isMentioned(_user.ID);
	}

	Message Message::send(BaseDiscordClient* client) {
		return client->sendMessage(channelID, content, !embeds.empty() ? embeds[0] : Embed(), tts);
	}

	Message Message::reply(BaseDiscordClient * client, std::string message, Embed embed, bool tts)
	{
		return client->sendMessage(channelID, message, embed, tts);
	}

	//const std::initializer_list<const char*const> Emoji::fields = {
	//	"id", "name", "roles", "user", "require_colons", "managed"
	//};

	//Emoji::Emoji() : requireColons(false), managed(false) {
	//}

	Emoji::~Emoji() {
	}

	Emoji::Emoji(const json::Value& json) :
		Emoji(json::fromJSON<Emoji>(json)) {
	}

	Emoji::Emoji(const nonstd::string_view& json) :
		Emoji(json::fromJSON<Emoji>(json)) {
	}

	//Emoji::Emoji(const json::Values values) :
	//	//variable     modifier             value                     felid
	//	Parent        (                    values[index(fields, "id"            )] ),
	//	name          (                    values[index(fields, "name"          )] ),
	//	roles         (JSON_getArray<Role>(values[index(fields, "roles"         )])),
	//	user          (                    values[index(fields, "user"          )] ),
	//	requireColons (getBool(            values[index(fields, "require_colons")])),
	//	managed       (getBool(            values[index(fields, "managed"       )]))
	//{}

	//Emoji::Emoji(const std::string * rawJson) : Emoji(json::getValues(rawJson->c_str(), fields)) {}

	//Emoji::Emoji(const json::Value& rawJSON) : Emoji(json::getValues(rawJSON.data(), fields)) {}

	//const std::initializer_list<const char*const> Reaction::fields {
	//	"count", "me", "emoji"
	//};

	//Reaction::Reaction() : count(0), me(false) {
	//}

	Reaction::~Reaction() {
	}

	Reaction::Reaction(const json::Value& json) :
		Reaction(json::fromJSON<Reaction>(json)) {
	}

	Reaction::Reaction(const nonstd::string_view& json) :
		Reaction(json::fromJSON<Reaction>(json)) {
	}

	//Reaction::Reaction(const json::Values values) :
	//	//variable modifier value                 felid
	//	count(toInt    (values[index(fields, "count")])),
	//	me   (getBool  (values[index(fields, "me"   )])),
	//	emoji(          values[index(fields, "emoji")])
	//{}

	//Reaction::Reaction(const std::string * rawJson) : Reaction(json::getValues(rawJson->c_str(), fields)) {}

	//Reaction::Reaction(const json::Value& rawJSON) : Reaction(json::getValues(rawJSON.data(), fields)) {}
}