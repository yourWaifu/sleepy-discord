#pragma once
#include <string>
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct Attachment : public DiscordObject {
	public:
		Attachment();
		~Attachment();
		Attachment(const std::string * rawJSON);
		std::string id;
		std::string filename;
		uint64_t size;
		std::string url;
		std::string proxy_url;
		unsigned long height;	//nullable
		unsigned long width;	//nullable
	};
}
