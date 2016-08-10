#pragma once
#include <string>
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct EmbedThumbnail : public DiscordObject {
	public:
		void fillOut(JSON_object* _JSON_object);
		void fillOut(const char* name, void* value);
		std::string url;
		std::string proxy_url;
		double* height;
		double* width;
	};

	struct EmbedProvider : public DiscordObject {
	public:
		void fillOut(JSON_object* _JSON_object);
		void fillOut(const char* name, void* value);
		std::string name;
		std::string url;
	};

	struct Embed : public DiscordObject {
	public:
		void fillOut(JSON_object* _JSON_object);
		void fillOut(const char* name, void* value);
		std::string title;
		std::string type;
		std::string description;
		std::string url;
		EmbedThumbnail thumbnail;
		EmbedProvider provider;
	};
}
