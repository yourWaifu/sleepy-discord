#pragma once
#include <tuple>
#include <memory>
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
		std::vector<Snowflake<Role>> roles;
		User user;	//optional
		bool requireColons = false;
		bool managed = false;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair                           (&Emoji::ID           , "id"            , json::NULLABLE_FIELD),
				json::pair                           (&Emoji::name         , "name"          , json::NULLABLE_FIELD),
				json::pair<json::ContainerTypeHelper>(&Emoji::roles        , "roles"         , json::OPTIONAL_FIELD),
				json::pair                           (&Emoji::user         , "user"          , json::OPTIONAL_FIELD),
				json::pair                           (&Emoji::requireColons, "require_colons", json::OPTIONAL_FIELD),
				json::pair                           (&Emoji::managed      , "managed"       , json::OPTIONAL_FIELD)
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

	struct StickerPack : public IdentifiableDiscordObject<StickerPack> {
	public:
		StickerPack() = default;
		~StickerPack();
		StickerPack(const json::Value & json);
		StickerPack(const nonstd::string_view & json);

		JSONStructStart
			std::make_tuple(
				json::pair(&StickerPack::ID, "id", json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};

	struct Sticker : public IdentifiableDiscordObject<Sticker> {
	public:
		Sticker() = default;
		~Sticker();
		Sticker(const json::Value & json);
		Sticker(const nonstd::string_view & json);
		Snowflake<StickerPack> packID;
		std::string name;
		std::string description;
		std::string tags;
		enum class Type : int {
			NONE = 0,
			PNG = 1,
			APNG = 2,
			LOTTIE = 3
		} format;
		
		JSONStructStart
			std::make_tuple(
				json::pair                      (&Sticker::ID             , "id"              , json::REQUIRIED_FIELD),
				json::pair                      (&Sticker::name           , "name"            , json::OPTIONAL_FIELD ),
				json::pair                      (&Sticker::description    , "description"     , json::OPTIONAL_FIELD ),
				json::pair                      (&Sticker::tags           , "tags"            , json::OPTIONAL_FIELD ),
				json::pair<json::EnumTypeHelper>(&Sticker::format         , "format_type"     , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

	//forward declearion
	class BaseDiscordClient;
	struct Server;
	struct Message;

	struct MessageReference {
	public:
		MessageReference() = default;
		~MessageReference() = default;
		MessageReference(const json::Value& json);
		MessageReference(const nonstd::string_view& json);
		MessageReference(const Message& message);

		Snowflake<Message> messageID;
		Snowflake<Channel> channelID;
		Snowflake<Server> serverID;

		JSONStructStart
			std::make_tuple(
				json::pair(&MessageReference::messageID, "message_id", json::OPTIONAL_FIELD),
				json::pair(&MessageReference::channelID, "channel_id", json::OPTIONAL_FIELD),
				json::pair(&MessageReference::serverID , "guild_id"  , json::OPTIONAL_FIELD)
			);
		JSONStructEnd

		inline const bool empty() const {
			return messageID.empty() && channelID.empty() && serverID.empty();
		}
	};

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
		Message reply(BaseDiscordClient * client, std::string message,
			Embed embed = Embed()
		);

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
			DEFAULT                                =  0,
			RECIPIENT_ADD                          =  1,
			RECIPIENT_REMOVE                       =  2,
			CALL                                   =  3,
			CHANNEL_NAME_CHANGE                    =  4,
			CHANNEL_ICON_CHANGE                    =  5,
			CHANNEL_PINNED_MESSAGE                 =  6,
			GUILD_MEMBER_JOIN                      =  7,
			USER_PREMIUM_GUILD_SUBSCRIPTION        =  8,
			USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1 =  9,
			USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2 = 10,
			USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3 = 11,
			CHANNEL_FOLLOW_ADD                     = 12,
			GUILD_DISCOVERY_DISQUALIFIED           = 14,
			GUILD_DISCOVERY_REQUALIFIED            = 15,
			REPLY                                  = 19
		} type = DEFAULT;
		std::vector<Sticker> stickers;
		MessageReference messageReference;
		std::shared_ptr<Message> referencedMessage;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair                           (&Message::ID               , "id"                , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::channelID        , "channel_id"        , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::serverID         , "guild_id"          , json::OPTIONAL_FIELD         ),
				json::pair                           (&Message::author           , "author"            , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::content          , "content"           , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::member           , "member"            , json::OPTIONAL_FIELD         ),
				json::pair                           (&Message::timestamp        , "timestamp"         , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::editedTimestamp  , "edited_timestamp"  , json::NULLABLE_FIELD         ),
				json::pair                           (&Message::tts              , "tts"               , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::mentionEveryone  , "mention_everyone"  , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::mentions         , "mentions"          , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::mentionRoles     , "mention_roles"     , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::attachments      , "attachments"       , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::embeds           , "embeds"            , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::reactions        , "reactions"         , json::OPTIONAL_FIELD         ),
				json::pair                           (&Message::pinned           , "pinned"            , json::REQUIRIED_FIELD        ),
				json::pair                           (&Message::webhookID        , "webhook_id"        , json::OPTIONAL_FIELD         ),
				json::pair<json::EnumTypeHelper     >(&Message::type             , "type"              , json::REQUIRIED_FIELD        ),
				json::pair<json::ContainerTypeHelper>(&Message::stickers         , "stickers"          , json::OPTIONAL_FIELD         ),
				json::pair                           (&Message::messageReference , "message_reference" , json::OPTIONAL_FIELD         ),
				json::pair<json::SmartPtrTypeHelper >(&Message::referencedMessage, "referenced_message", json::OPTIONAL_FIELD         )
			);
		JSONStructEnd
	};

	inline MessageReference::MessageReference(const Message& message) :
		messageID(message.ID),
		channelID(message.channelID),
		serverID(message.serverID)
	{}

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

	struct AllowedMentions {
	public:
		std::vector<std::string> parse;
		std::vector<Snowflake<Role>> roles;
		std::vector<Snowflake<User>> users;
		enum class MentionReplierFlag : char {
			NotSet = -2,
			WillNotMentionReply = false,
			MentionReply = true
		};
		MentionReplierFlag repliedUser = MentionReplierFlag::NotSet;

		JSONStructStart
			std::make_tuple(
				json::pair<json::ContainerTypeHelper>(&AllowedMentions::parse      , "parse"       , json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&AllowedMentions::roles      , "roles"       , json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&AllowedMentions::users      , "users"       , json::OPTIONAL_FIELD),
				json::pair<json::EnumTypeHelper     >(&AllowedMentions::repliedUser, "replied_user", json::OPTIONAL_FIELD)
			);
		JSONStructEnd

		inline const bool empty() const {
			return parse.empty() && repliedUser == MentionReplierFlag::NotSet;
		}
	};

	template<>
	struct GetDefault<AllowedMentions::MentionReplierFlag> {
		static inline const AllowedMentions::MentionReplierFlag get() {
			return AllowedMentions::MentionReplierFlag::NotSet;
		} 
	};

	template<>
	struct GetEnumBaseType<AllowedMentions::MentionReplierFlag> {
		//this makes the json wrapper know to use getBool instead of getInt
		using Value = bool; 
	};

	struct SendMessageParams : public DiscordObject {
	public:
		Snowflake<Channel> channelID;
		std::string content = {};
		bool tts = false;
		Embed embed = Embed::Flag::INVALID_EMBED;
		AllowedMentions allowedMentions;
		MessageReference messageReference;
		JSONStructStart
			std::make_tuple(
				json::pair(&SendMessageParams::content         , "content"          , json::REQUIRIED_FIELD),
				json::pair(&SendMessageParams::tts             , "tts"              , json::OPTIONAL_FIELD ),
				json::pair(&SendMessageParams::embed           , "embed"            , json::OPTIONAL_FIELD ),
				json::pair(&SendMessageParams::allowedMentions , "allowed_mentions" , json::OPTIONAL_FIELD ),
				json::pair(&SendMessageParams::messageReference, "message_reference", json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};
}