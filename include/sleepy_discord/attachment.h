#pragma once
#include <string>
#include "snowflake.h"
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct Attachment : public IdentifiableDiscordObject<Attachment> {
	public:
		Attachment();
		~Attachment();
		Attachment(const std::string * rawJSON);
		Attachment(const std::vector<std::string> values);
		std::string filename;
		uint64_t size;
		std::string url;
		std::string proxy_url;
		unsigned long height;	//nullable
		unsigned long width;	//nullable
	private:
		const static std::initializer_list<const char*const> fields;
	};
}
