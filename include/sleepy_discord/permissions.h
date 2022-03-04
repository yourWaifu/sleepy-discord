#pragma once
#include <string>
#include <algorithm>
#include <cstdlib>
#include "discord_object_interface.h"
#include "snowflake.h"
#include "user.h"

//source: discord api docs | /topics/Permissions.md | Nov 16

namespace SleepyDiscord {
	struct ServerMember;
	struct Server;
	struct Overwrite;
	struct Channel;

	using PermissionRaw = uint64_t;
	enum Permission : PermissionRaw {
		CREATE_INSTANT_INVITE = 0x00000001, //Allows creation of instant invites
		KICK_MEMBERS    /**/  = 0x00000002, //Allows kicking members
		BAN_MEMBERS     /**/  = 0x00000004, //Allows banning members
		ADMINISTRATOR   /**/  = 0x00000008, //Allows all permissions and bypasses channel permission overwrites
		MANAGE_CHANNELS /**/  = 0x00000010, //Allows management and editing of channels
		MANAGE_GUILD    /**/  = 0x00000020, //Allows management and editing of the guild
		ADD_REACTIONS         = 0x00000040, //Allows for the addition of reactions to messages
		VIEW_AUDIT_LOG        = 0x00000080, //Allows for viewing of audit logs
		VIEW_CHANNEL          = 0x00000400, //Allows guild members to view a channel
		SEND_MESSAGES         = 0x00000800, //Allows for sending messages in a channel.
		SEND_TTS_MESSAGES     = 0x00001000, //Allows for sending of /tts messages
		MANAGE_MESSAGES /**/  = 0x00002000, //Allows for deletion of other users messages
		EMBED_LINKS           = 0x00004000, //Links sent by this user will be auto - embedded
		ATTACH_FILES          = 0x00008000, //Allows for uploading images and files
		READ_MESSAGE_HISTORY  = 0x00010000, //Allows for reading of message history
		MENTION_EVERYONE      = 0x00020000, //Allows for using the @everyone tag to notify all users in a channel, and the @here tag to notify all online users in a channel
		USE_EXTERNAL_EMOJIS   = 0x00040000, //Allows the usage of custom emojis from other servers
		CONNECT               = 0x00100000, //Allows for joining of a voice channel
		SPEAK                 = 0x00200000, //Allows for speaking in a voice channel
		MUTE_MEMBERS          = 0x00400000, //Allows for muting members in a voice channel
		DEAFEN_MEMBERS        = 0x00800000, //Allows for deafening of members in a voice channel
		MOVE_MEMBERS          = 0x01000000, //Allows for moving of members between voice channels
		USE_VAD               = 0x02000000, //Allows for using voice - activity - detection in a voice channel
		PRIORITY_SPEAKER      = 0x00000100, //Allows for using priority speaker in a voice channel
		CHANGE_NICKNAME       = 0x04000000, //Allows for modification of own nickname
		MANAGE_NICKNAMES      = 0x08000000, //Allows for modification of other users nicknames
		MANAGE_ROLES    /**/  = 0x10000000, //Allows management and editing of roles
		MANAGE_WEBHOOKS /**/  = 0x20000000, //Allows management and editing of webhooks
		MANAGE_EMOJIS   /**/  = 0x40000000, //Allows management and editing of emojis
		//              /**/ These permissions require the owner account to use two-factor authentication when used on a guild that has server-wide 2FA enabled.

		NONE                  = 0x000000000, //this permission doens't exist, I made it up
		ALL                   = 0xFFFFFFFFF,

		READ_MESSAGES = VIEW_CHANNEL,
	};
	
	inline constexpr Permission toPermission(const int64_t& permission) {
		return static_cast<Permission>(permission);
	}

	inline Permission toPermission(const json::Value& value) {
		return toPermission(value.GetInt64());
	}

	inline constexpr Permission operator|(const Permission& a, const Permission& b) {
		return static_cast<Permission>(static_cast<int64_t>(a) | static_cast<int64_t>(b));
	}

	inline constexpr Permission operator&(const Permission& a, const Permission& b) {
		return static_cast<Permission>(static_cast<int64_t>(a) & static_cast<int64_t>(b));
	}

	inline constexpr Permission operator^(const Permission& a, const Permission& b) {
		return static_cast<Permission>(static_cast<int64_t>(a) ^ static_cast<int64_t>(b));
	}
	
	inline constexpr bool hasPremission(const Permission& target, const Permission& permission) {
		return (target & permission) == permission;
	}

	Permission getBasePermissions(const Server& server, const ServerMember& member);
	void handleOverwrite(Permission& target, const Permission& allow, const Permission& deny);
	void handleOverwrite(Permission& target, const Overwrite& overwrite);
	Permission overwritePermissions(const Permission basePermissions, const Server& server, const ServerMember& member, const Channel& channel);
	Permission getPermissions(const Server& server, const ServerMember& member, const Channel& channel);

	template<class Type>
	struct UInt64StrTypeHelper {
		static inline Permission toType(const json::Value& value) {
			//c++11 has strtoull which requires a std::string
			//c++17 has from_chars tho
			return Type(std::strtoull(value.GetString(), nullptr, 10));
		}
		static inline json::Value fromType(const Type& value, json::Value::AllocatorType& alloc) {
			std::string valueStr = std::to_string(static_cast<uint64_t>(value));
			//we need to allocate some memory so that the value isn't invalid after returning
			return json::Value(valueStr.c_str(), valueStr.length(), alloc); //allocates and copies
		}
		static inline bool empty(const Type& value) {;
			return value == Type(0);
		}
		static inline bool isType(const json::Value& value) {
			return value.IsString();
		}
	};

	struct User;

	/*
	Role Structure

	Field	    Type	  Description
	id          snowflake role id
	name	    string    role name
	color       integer	  integer representation of hexadecimal color code
	hoist       bool      if this role is pinned in the user listing
	position	integer   position of this role
	permissions	integer   permission bit set
	managed     bool      whether this role is managed by an integration
	mentionable bool      whether this role is mentionable
	*/
	struct Role : public IdentifiableDiscordObject<Role> {
		Role() = default;
		~Role() {}
		//Role(const std::string * rawJson);
		Role(const json::Value & rawJSON);
		Role(const nonstd::string_view& json) :
			Role(json::fromJSON<Role>(json)) {}
		//Role(const json::Values values);
		std::string name;
		int color = -1;
		bool hoist = false;
		int position = 0;
		Permission permissions = NONE;
		bool managed = false;
		bool mentionable = false;

		struct Tags : public DiscordObject {
			Tags() = default;
			~Tags() {}
			Tags(const json::Value& rawJSON);
			Tags(const nonstd::string_view& json) :
				Tags(json::fromJSON<Tags>(json)) {}
			Snowflake<User> botID;
			Snowflake<DiscordObject> integrationID;

			inline const bool empty() const {
				return botID.empty() && integrationID.empty();
			}

			JSONStructStart
				std::make_tuple(
					json::pair(&Tags::botID        , "bot_id"        , json::OPTIONAL_FIELD),
					json::pair(&Tags::integrationID, "integration_id", json::OPTIONAL_FIELD)
				);
			JSONStructEnd
		};
		Tags tags;

		inline bool operator==(Role& right) {
			return ID == right.ID;
		}

		JSONStructStart
			std::make_tuple(
				json::pair                     (&Role::ID         , "id"         , json::REQUIRIED_FIELD),
				json::pair                     (&Role::name       , "name"       , json::REQUIRIED_FIELD),
				json::pair                     (&Role::color      , "color"      , json::REQUIRIED_FIELD),
				json::pair                     (&Role::hoist      , "hoist"      , json::REQUIRIED_FIELD),
				json::pair                     (&Role::position   , "position"   , json::REQUIRIED_FIELD),
				json::pair<UInt64StrTypeHelper>(&Role::permissions, "permissions", json::REQUIRIED_FIELD),
				json::pair                     (&Role::managed    , "managed"    , json::REQUIRIED_FIELD),
				json::pair                     (&Role::mentionable, "mentionable", json::REQUIRIED_FIELD),
				json::pair                     (&Role::tags       , "tags"       , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};
}