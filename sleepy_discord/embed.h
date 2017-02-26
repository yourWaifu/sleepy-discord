#pragma once
#include <string>
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct EmbedThumbnail : public DiscordObject {
	public:
		EmbedThumbnail();
		EmbedThumbnail(const std::string * rawJSON);
		std::string url;
		std::string proxy_url;
		unsigned long height;
		unsigned long width;
	};

	struct EmbedProvider : public DiscordObject {
	public:
		EmbedProvider();
		EmbedProvider(const std::string * rawJSON);
		std::string name;
		std::string url;
	};

	struct Embed : public DiscordObject {
	public:
		Embed();
		Embed(const std::string * rawJSON);
		std::string title;
		std::string type;
		std::string description;
		std::string url;
		EmbedThumbnail thumbnail;
		EmbedProvider provider;
	};
}
