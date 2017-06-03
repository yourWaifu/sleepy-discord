#include "channel.h"

namespace SleepyDiscord {
	Channel::Channel(const std::string * rawJson) {
		//default values
		bitrate = 0;
		userLimit = 0;   //this only matters on voice channels
		topic = "";
		lastMessage_id = "";

		std::vector<std::string> values = json::getValues(rawJson->c_str(),
			{ "id", "guild_id", "name", "type", "position", "is_private",
			"permission_overwrites", "topic", "last_message_id", "bitrate", "user_limit" });
		id                   =            values[0] ;
		guild_id             =            values[1] ;
		name                 =            values[2] ;
		type                 =            values[3] ;
		position             = std::stoi( values[4]);
		isPrivate            = getBool(   values[5]);
		permissionOverwrites = Overwrite(&values[6]);
		topic                =            values[7] ;
		lastMessage_id       =            values[8] ;
		bitrate              = std::stoi( values[9]);
		userLimit            = std::stoi( values[9]);
	}

	Channel::~Channel() {

	}

	DirectMessageChannel::~DirectMessageChannel() {

	}

	DirectMessageChannel::DirectMessageChannel(const std::string * rawJson) {
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "is_private", "recipient", "last_message_id" });
		unsigned int i = -1;
		id             = values[++i];
		is_private     = getBool(values[++i]);
		recipient      = User(&values[++i]);
		lastMessage_id = values[++i];
	}

	Overwrite::Overwrite(const std::string * rawJson) {
		std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "type", "allow", "deny" });
		unsigned int i = -1;
		id    = values[++i];
		type  = values[++i];
		allow = std::stoi(values[++i]);
		deny  = std::stoi(values[++i]);
	}
}