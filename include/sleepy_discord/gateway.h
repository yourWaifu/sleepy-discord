#pragma once
#include "discord_object_interface.h"
#include "user.h"
#include "channel.h"

namespace SleepyDiscord {
	enum GameType {
		Playing = 0,  //calling this Game causes issues
		Streaming = 1
	};

	enum Status {
		statusError = 0,
		online         ,
		doNotDisturb   ,
		idle           ,
		invisuble      ,
		offline        
	};

	struct Game : DiscordObject {
		std::string name = "";
		GameType type;
		std::string url = ""; //used when type is Streaming
	};

	struct Ready : public DiscordObject {
		Ready(const std::string * rawJSON);
		Ready(const std::vector<std::string> values);
		int v;	//gateway protocol version
		User user;
		std::vector<DMChannel> privateChannels;
		//std::vector<UnavailableServer> servers;
		std::string sessionID;
		std::vector<std::string> trace;
	private:
		const static std::initializer_list<const char*const> fields;
	};
}