#pragma once
#include <string>
#include "discord_object_interface.h"

namespace SleepyDiscord {
	class Channel : public DiscordObject {
	public:
		Channel();
		~Channel();
		Channel(DiscordClient* client, JSON * jsonMessage);
		Channel(JSON_object * messageJSONObject);
		void fillOut(JSON_object * _JSON_object);
		void fillOut(const char* name, void * value);
		std::string id;
		std::string guild_id;
		std::string name;
		std::string type;
		int position;
		bool is_private;
		//Overwrite* permissionOverwrites;
		std::string topic;
		std::string lastMessage_id;
		int bitrate;
		int userLimit;
	};

	
}