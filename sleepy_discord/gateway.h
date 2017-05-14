#pragma once
#include "discord_object_interface.h"

namespace SleepyDiscord {
	enum GameType {
		Playing = 0,  //calling this Game causes issues
		Streaming = 1
	};

	enum Status {
		idle,
		online,
		offline,
		null
	};

	struct Game : DiscordObject {
		std::string name = "";
		GameType type;
		std::string url = ""; //used when type is Streaming
	};
}