#pragma once
#include <string>
#include "user.h"
#include "permissions.h"
#include "snowflake.h"

namespace SleepyDiscord {
	struct Overwrite : DiscordObject {
		Overwrite();
		Overwrite(const std::string * rawJson);
		Overwrite(const std::vector<std::string> values);
		~Overwrite() {}
		Snowflake<Overwrite> ID;
		std::string type;
		Permission allow;
		Permission deny;

		inline bool operator==(Overwrite& right) {
			return ID == right.ID;
		}
	private:
		const static std::initializer_list<const char*const> fields;
	};

	//forward declearion
	struct Server;
	struct Message;

	struct Channel : DiscordObject {
	public:
		Channel();
		Channel(const std::string * rawJson);
		Channel(const std::vector<std::string> values);
		~Channel();
		
		Snowflake<Channel> ID;
		enum ChannelType {
			CHANNEL_TYPE_NONE = -1,
			SERVER_TEXT       = 0,
			DM                = 1,
			SERVER_VOICE      = 2,
			GROUP_DM          = 3,
			SERVER_CATEGORY   = 4
		} type;
		Snowflake<Server>      serverID;             //optional,                  used in server       channels
		int                    position;             //optional,                  used in server       channels
		std::vector<Overwrite> permissionOverwrites; //optional,                  used in server       channels
		std::string            name;                 //optional,              not used in           DM channels
		std::string            topic;                //optional and nullable,     used in server  text channels
		bool                   isNSFW;               //optional,                  used in server       channels
		Snowflake<Message>     lastMessageID;        //optional,                  used in         text channels
		int                    bitrate;              //optional,                  used in        voice channels
		int                    userLimit;            //optional,                  used in        voice channels
		std::vector<User>      recipients;           //optional,                  used in all       DM channels
		std::string            icon;                 //optional and nullable,     used in group     DM channels
		Snowflake<User>        ownerID;              //optional,                  used in group     DM channels
		//Snowflake<>          applicationID;        //??????????????????????     used in group     DM channels
		Snowflake<Channel>     parentID;             //optional and nullable,     used in server       channels
		std::string            lastPinTimestamp;     //optional,                  used in         text channels

		inline bool operator==(Channel& right) {
			return ID == right.ID;
		}
	private:
		const static std::initializer_list<const char*const> fields;
	};
}