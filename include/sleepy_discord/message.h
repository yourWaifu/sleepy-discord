#pragma once
#include <tuple>
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
	struct Emoji : public IdentifiableDiscordObject<Emoji> {
	public:
		~Emoji();
		Emoji() = default;
		//Emoji(const std::string* rawJson);
		Emoji(const json::Value & rawJSON);
		Emoji(const nonstd::string_view& json);
		//Emoji(const json::Values values);
		std::string name;
		std::vector<Role> roles;
		User user;	//optional
		bool requireColons = false;
		bool managed = false;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair                           (&Emoji::ID           , "id"            , json::NULLABLE_FIELD ),
				json::pair                           (&Emoji::name         , "name"          , json::REQUIRIED_FIELD),
				json::pair<json::ContainerTypeHelper>(&Emoji::roles        , "roles"         , json::OPTIONAL_FIELD ),
				json::pair                           (&Emoji::user         , "user"          , json::OPTIONAL_FIELD ),
				json::pair                           (&Emoji::requireColons, "require_colons", json::OPTIONAL_FIELD ),
				json::pair                           (&Emoji::managed      , "managed"       , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};
	
	struct Reaction : public DiscordObject {
	public:
		Reaction() = default;
		~Reaction();
		//Reaction(const std::string * rawJson);
		Reaction(const json::Value & rawJSON);
		Reaction(const nonstd::string_view & json);
		//Reaction(const json::Values values);
		int count = 0;
		bool me = false;
		Emoji emoji;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&Reaction::count, "count", json::REQUIRIED_FIELD),
				json::pair(&Reaction::me   , "me"   , json::REQUIRIED_FIELD),
				json::pair(&Reaction::emoji, "emoji", json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};

	//forward declearion
	class BaseDiscordClient;
	struct Server;

	struct Message : public IdentifiableDiscordObject<Message> {
	public:
		Message() = default;
		~Message() = default;
		//Message(const json::Values values);
		//Message(const std::string * rawJson);
		Message(const json::Value& json);
		Message(const nonstd::string_view& json);
		//using DiscordObject::DiscordObject;
		bool startsWith(const std::string& test);
		std::size_t length();
		bool isMentioned(Snowflake<User> ID);
		bool isMentioned(User& _user);
		Message send(BaseDiscordClient * client);
		Message reply(BaseDiscordClient * client, std::string message, Embed embed = Embed(), bool tts = false);

		Snowflake<Channel> channelID;
		Snowflake<Server> serverID;
		User author;
		ServerMember member;
		std::string content;
		std::string timestamp;
		std::string editedTimestamp;
		bool tts = false;
		bool mentionEveryone = false;
		std::vector<User> mentions;
		std::vector<Snowflake<User>> mentionRoles;
		std::vector<Attachment> attachments;
		std::vector<Embed> embeds;
		std::vector<Reaction> reactions;
		bool pinned = false;
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
		} type = DEFAULT;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair                           (&Message::ID             , "id"              , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::channelID      , "channel_id"      , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::serverID       , "guild_id"        , json::OPTIONAL_FIELD         ),
				json::pair                           (&Message::author         , "author"          , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::content        , "content"         , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::member         , "member"          , json::OPTIONAL_FIELD         ),
				json::pair                           (&Message::timestamp      , "timestamp"       , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::editedTimestamp, "edited_timestamp", json::NULLABLE_FIELD         ),
				json::pair                           (&Message::tts            , "tts"             , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::mentionEveryone, "mention_everyone", json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::mentions       , "mentions"        , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::mentionRoles   , "mention_roles"   , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::attachments    , "attachments"     , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::embeds         , "embeds"          , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::reactions      , "reactions"       , json::OPTIONAL_FIELD         ),
				json::pair                           (&Message::pinned         , "pinned"          , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::webhookID      , "webhook_id"      , json::OPTIONAL_FIELD         ),
				json::pair<json::EnumTypeHelper     >(&Message::type           , "type"            , json::REQUIRIED_FIELD        )
			);
		JSONStructEnd
	};

	struct MessageRevisions {
		MessageRevisions(const json::Value& json) :
			messageID(json["id"]), channelID(json["channel_id"]), RevisionsJSON(json)
		{}
		inline void applyChanges(Message& outOfDateMessage) {
			assert(outOfDateMessage.ID == messageID);
			json::fromJSON(outOfDateMessage, RevisionsJSON);
		}
		Snowflake<Message> messageID;
		Snowflake<Channel> channelID;
		const json::Value& RevisionsJSON;
	};

	struct SendMessageParams : public DiscordObject {
	public:
		Snowflake<Channel> channelID;
		std::string content = {};
		bool tts = false;
		Embed embed = Embed::Flag::INVALID_EMBED;
		JSONStructStart
			std::make_tuple(
				json::pair(&SendMessageParams::content, "content", json::REQUIRIED_FIELD),
				json::pair(&SendMessageParams::tts    , "tts"    , json::OPTIONAL_FIELD ),
				json::pair(&SendMessageParams::embed  , "embed"  , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};
}