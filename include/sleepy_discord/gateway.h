#pragma once
#include "discord_object_interface.h"
#include "user.h"
#include "channel.h"
#include "server.h"

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
		Ready() = default;
		//Ready(const std::string * rawJSON);
		Ready(const json::Value & rawJSON);
		Ready(const nonstd::string_view & rawJSON);
		//Ready(const json::Values values);
		int v;	//gateway protocol version
		User user;
		std::list<Channel> privateChannels;
		std::list<UnavailableServer> servers;
		std::string sessionID;
		//std::vector<std::string> trace;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair                           (&Ready::v              , "v"               , json::REQUIRIED_FIELD),
				json::pair                           (&Ready::user           , "user"            , json::REQUIRIED_FIELD),
				json::pair<json::ContainerTypeHelper>(&Ready::privateChannels, "private_channels", json::REQUIRIED_FIELD),
				json::pair<json::ContainerTypeHelper>(&Ready::servers        , "guilds"          , json::REQUIRIED_FIELD),
				json::pair                           (&Ready::sessionID      , "session_id"      , json::REQUIRIED_FIELD)
				//This doesn't work anymore
				//json::pair(&Ready::trace          , json::toArray<std::string>      , "_trace"          , json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};
}