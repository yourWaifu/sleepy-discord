#pragma once
#include <string>
#include "user.h"
#include "permissions.h"
#include "snowflake.h"

namespace SleepyDiscord {
	struct Overwrite : IdentifiableDiscordObject<Overwrite> {
		Overwrite();
		Overwrite(const std::string * rawJson);
		Overwrite(const std::vector<std::string> values);
		~Overwrite() {}
		std::string type;
		Permission allow;
		Permission deny;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	//forward declearion
	struct Server;
	struct Message;

	struct Channel : IdentifiableDiscordObject<Channel> {
	public:
		Channel() {}
		Channel(const std::string * rawJson);
		Channel(const std::vector<std::string> values);
		~Channel();
		
		enum ChannelType {
			SERVER_TEXT     = 0,
			DM              = 1,
			SERVER_VOICE    = 2,
			GROUP_DM        = 3,
			SERVER_CATEGORY = 4
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
	private:
		const static std::initializer_list<const char*const> fields;
	};

	struct DirectMessageChannel : public DiscordObject {
		DirectMessageChannel() {}
		DirectMessageChannel(const std::string * rawJson);
		DirectMessageChannel(const std::vector<std::string> values);
		~DirectMessageChannel();
		Snowflake<DirectMessageChannel> ID;
		bool is_private;
		User recipient;
		Snowflake<Message> lastMessageID;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	typedef DirectMessageChannel DMChannel;
}