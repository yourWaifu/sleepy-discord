#pragma once
#include <string>
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct EmbedThumbnail : public DiscordObject {
	public:
		EmbedThumbnail();
		EmbedThumbnail(const std::string * rawJSON);
		EmbedThumbnail(const std::vector<std::string> values);
		std::string url;
		std::string proxy_url;
		unsigned long height;
		unsigned long width;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	struct EmbedProvider : public DiscordObject {
	public:
		EmbedProvider();
		EmbedProvider(const std::string * rawJSON);
		EmbedProvider(const std::vector<std::string> values);
		std::string name;
		std::string url;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	struct Embed : public DiscordObject {
	public:
		Embed();
		Embed(const std::string * rawJSON);
		Embed(const std::vector<std::string> values);
		std::string title;
		std::string type;
		std::string description;
		std::string url;
		EmbedThumbnail thumbnail;
		EmbedProvider provider;
	private:
		const static std::initializer_list<const char*const> fields;
	};
}
