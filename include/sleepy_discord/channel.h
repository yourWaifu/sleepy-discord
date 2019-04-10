#pragma once
#include <string>
#include "user.h"
#include "permissions.h"
#include "snowflake.h"

namespace SleepyDiscord {
	struct Overwrite : IdentifiableDiscordObject<Overwrite> {
		Overwrite() = default;
		//Overwrite(const std::string * rawJson);
		Overwrite(const json::Value & rawJSON);
		Overwrite(const nonstd::string_view & rawJSON);
		//Overwrite(const json::Values values);
		~Overwrite() {}
		std::string type;
		Permission allow = Permission::NONE;
		Permission deny = Permission::NONE;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair<json::ClassTypeHelper>(&Overwrite::ID   , "id"   , json::REQUIRIED_FIELD),
				json::pair<json::ClassTypeHelper>(&Overwrite::type , "type" , json::REQUIRIED_FIELD),
				json::pair<json::EnumTypeHelper >(&Overwrite::allow, "allow", json::REQUIRIED_FIELD),
				json::pair<json::EnumTypeHelper >(&Overwrite::deny , "deny" , json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};

	//forward declearion
	struct Server;
	struct Message;

	struct Channel : IdentifiableDiscordObject<Channel> {
	public:
		Channel() = default;
		//Channel(const std::string * rawJson);
		Channel(const json::Value & rawJSON);
		Channel(const nonstd::string_view & rawJSON);
		//Channel(const json::Values values);
		~Channel();
		
		enum ChannelType {
			CHANNEL_TYPE_NONE = -1,
			SERVER_TEXT       = 0,
			DM                = 1,
			SERVER_VOICE      = 2,
			GROUP_DM          = 3,
			SERVER_CATEGORY   = 4,
			GUILD_NEWS        = 5,
			GUILD_STORE       = 6
		} type = CHANNEL_TYPE_NONE;
		Snowflake<Server>      serverID;             //optional,                  used in server       channels
		int                    position = 0;         //optional,                  used in server       channels
		std::vector<Overwrite> permissionOverwrites; //optional,                  used in server       channels
		std::string            name;                 //optional,              not used in           DM channels
		std::string            topic;                //optional and nullable,     used in server  text channels
		bool                   isNSFW = false;       //optional,                  used in server       channels
		Snowflake<Message>     lastMessageID;        //optional,                  used in         text channels
		int                    bitrate = 0;          //optional,                  used in        voice channels
		int                    userLimit = 0;        //optional,                  used in        voice channels
		std::vector<User>      recipients;           //optional,                  used in all       DM channels
		std::string            icon;                 //optional and nullable,     used in group     DM channels
		Snowflake<User>        ownerID;              //optional,                  used in group     DM channels
		//Snowflake<>          applicationID;        //??????????????????????     used in group     DM channels
		Snowflake<Channel>     parentID;             //optional and nullable,     used in server       channels
		std::string            lastPinTimestamp;     //optional,                  used in         text channels

		//const static std::initializer_list<const char*const> fields;

		JSONStructStart
			std::make_tuple(
				json::pair                           (&Channel::ID                   , "id"                   , json::REQUIRIED_FIELD        ),
				json::pair<json::EnumTypeHelper     >(&Channel::type                 , "type"                 , json::REQUIRIED_FIELD        ),
				json::pair                           (&Channel::serverID             , "guild_id"             , json::OPTIONAL_FIELD         ),
				json::pair                           (&Channel::position             , "position"             , json::OPTIONAL_FIELD         ),
				json::pair<json::ContainerTypeHelper>(&Channel::permissionOverwrites , "permission_overwrites", json::OPTIONAL_FIELD         ),
				json::pair                           (&Channel::name                 , "name"                 , json::OPTIONAL_FIELD         ),
				json::pair                           (&Channel::topic                , "topic"                , json::OPTIONAL_NULLABLE_FIELD),
				json::pair                           (&Channel::isNSFW               , "nsfw"                 , json::OPTIONAL_FIELD         ),
				json::pair                           (&Channel::lastMessageID        , "last_message_id"      , json::OPTIONAL_NULLABLE_FIELD),
				json::pair                           (&Channel::bitrate              , "bitrate"              , json::OPTIONAL_FIELD         ),
				json::pair                           (&Channel::userLimit            , "user_limit"           , json::OPTIONAL_FIELD         ),
				json::pair<json::ContainerTypeHelper>(&Channel::recipients           , "recipients"           , json::OPTIONAL_FIELD         ),
				json::pair                           (&Channel::icon                 , "icon"                 , json::OPTIONAL_NULLABLE_FIELD),
				json::pair                           (&Channel::ownerID              , "owner_id"             , json::OPTIONAL_FIELD         ),
				json::pair                           (&Channel::parentID             , "parent_id"            , json::OPTIONAL_NULLABLE_FIELD),
				json::pair                           (&Channel::lastPinTimestamp     , "last_pin_timestamp"   , json::OPTIONAL_FIELD         )
			);
		JSONStructEnd
	};
}