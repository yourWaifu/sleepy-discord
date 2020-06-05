#pragma once
#include "webhook.h"
#include "user.h"
#include "server.h"
#include <tuple>
#include <utility>

namespace SleepyDiscord {
	enum class AuditLogEvent {
		GUILD_UPDATE             = 1,
		CHANNEL_CREATE           = 10,
		CHANNEL_UPDATE           = 11,
		CHANNEL_DELETE           = 12,
		CHANNEL_OVERWRITE_CREATE = 13,
		CHANNEL_OVERWRITE_UPDATE = 14,
		CHANNEL_OVERWRITE_DELETE = 15,
		MEMBER_KICK              = 20,
		MEMBER_PRUNE             = 21,
		MEMBER_BAN_ADD           = 22,
		MEMBER_BAN_REMOVE        = 23,
		MEMBER_UPDATE            = 24,
		MEMBER_ROLE_UPDATE       = 25,
		ROLE_CREATE              = 30,
		ROLE_UPDATE              = 31,
		ROLE_DELETE              = 32,
		INVITE_CREATE            = 40,
		INVITE_UPDATE            = 41,
		INVITE_DELETE            = 42,
		WEBHOOK_CREATE           = 50,
		WEBHOOK_UPDATE           = 51,
		WEBHOOK_DELETE           = 52,
		EMOJI_CREATE             = 60,
		EMOJI_UPDATE             = 61,
		EMOJI_DELETE             = 62,
		MESSAGE_DELETE           = 72
	};

	template<class _ObjectChanged, class _Type>
	struct AuditLogKeyData {
	public:
		using ObjectChanged = _ObjectChanged;
		using Type = _type;
	};

	template<typename template<class, class> Data, ObjectChanged, Type>
	using AuditLogKey = std::pair<std::string, Data<ObjectChanged, Type>>; 

	struct AuditLog {
	public:

		const static auto keys = std::make_tuple(
			AuditLogKey("name"                         , AuditLogKeyData<Server , std::string           >()),
			AuditLogKey("icon_hash"                    , AuditLogKeyData<Server , std::string           >()),
			AuditLogKey("splash_hash"                  , AuditLogKeyData<Server , std::string           >()),
			AuditLogKey("owner_id"                     , AuditLogKeyData<Server , Snowflake<User>       >()),
			AuditLogKey("region"                       , AuditLogKeyData<Server , std::string           >()),
			AuditLogKey("afk_channel_id"               , AuditLogKeyData<Server , Snowflake<Channel>    >()),
			AuditLogKey("afk_timeout"                  , AuditLogKeyData<Server , int                   >()),
			AuditLogKey("mfa_level"                    , AuditLogKeyData<Server , int                   >()),
			AuditLogKey("verification_level"           , AuditLogKeyData<Server , int                   >()),
			AuditLogKey("explicit_content_filter"      , AuditLogKeyData<Server , int                   >()),
			AuditLogKey("default_message_notifications", AuditLogKeyData<Server , int                   >()),
			AuditLogKey("vanity_url_code"              , AuditLogKeyData<Server , std::string           >()),
			AuditLogKey("$add"                         , AuditLogKeyData<Server , std::vector<Role>     >()),
			AuditLogKey("$remove"                      , AuditLogKeyData<Server , std::vector<Role>     >()),
			AuditLogKey("prune_delete_days"            , AuditLogKeyData<Server , int                   >()),
			AuditLogKey("widget_enabled"               , AuditLogKeyData<Server , bool                  >()),
			AuditLogKey("widget_channel_id"            , AuditLogKeyData<Server , Snowflake<Channel>    >()),
			AuditLogKey("position"                     , AuditLogKeyData<Channel, int                   >()),
			AuditLogKey("topic"                        , AuditLogKeyData<Channel, std::string           >()),
			AuditLogKey("bitrate"                      , AuditLogKeyData<Channel, int                   >()),
			AuditLogKey("permission_overwrites"        , AuditLogKeyData<Channel, std::vector<Overwrite>>()),
			AuditLogKey("nsfw"                         , AuditLogKeyData<Channel, bool                  >()),
			AuditLogKey("application_id"               , AuditLogKeyData<Channel, std::string           >()), //to do use Snowflake
			AuditLogKey("permissions"                  , AuditLogKeyData<Role   , int                   >()),
			AuditLogKey("color"                        , AuditLogKeyData<Role   , int                   >()),
			AuditLogKey("hoist"                        , AuditLogKeyData<Role   , bool                  >()),
			AuditLogKey("mentionable"                  , AuditLogKeyData<Role   , bool                  >()),
			AuditLogKey("allow"                        , AuditLogKeyData<Role   , int                   >()),
			AuditLogKey("deny"                         , AuditLogKeyData<Role   , int                   >()),
			AuditLogKey("code"                         , AuditLogKeyData<Invite , std::string           >()),
			AuditLogKey("channel_id"                   , AuditLogKeyData<Invite , Snowflake<Channel>    >()),
			AuditLogKey("inviter_id"                   , AuditLogKeyData<Invite , Snowflake<User>       >()),
			AuditLogKey("max_uses"                     , AuditLogKeyData<Invite , int                   >()),
			AuditLogKey("uses"                         , AuditLogKeyData<Invite , int                   >()),
			AuditLogKey("max_age"                      , AuditLogKeyData<Invite , int                   >()),
			AuditLogKey("temporary"                    , AuditLogKeyData<Invite , bool                  >()),
			AuditLogKey("deaf"                         , AuditLogKeyData<User   , bool                  >()),
			AuditLogKey("mute"                         , AuditLogKeyData<User   , bool                  >()),
			AuditLogKey("nick"                         , AuditLogKeyData<User   , std::string           >()),
			AuditLogKey("avatar_hash"                  , AuditLogKeyData<User   , std::string           >()),
			AuditLogKey("id"                           , AuditLogKeyData<void   , Snowflake::RawType    >()),
			//to do: type doesn't really fit into the above format
		);
	};
}