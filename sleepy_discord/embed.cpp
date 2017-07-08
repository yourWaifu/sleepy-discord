#include "embed.h"

namespace SleepyDiscord {
	Embed::Embed() {
	}

	Embed::Embed(const std::string * rawJSON) {
		std::vector<std::string> values = json::getValues(rawJSON->c_str(),
			{ "title", "type", "description", "url", "thumbnail", "provider" });
		title = values[0];
		type = values[1];
		description = values[2];
		url = values[3];
		if (isSpecified(values[4])) thumbnail = EmbedThumbnail(&values[4]);	//if it doesn't have a thumbnail, it doesn't need to make one
		if (isSpecified(values[5])) provider = EmbedProvider(&values[5]);
	}

	EmbedThumbnail::EmbedThumbnail() {
	}

	EmbedThumbnail::EmbedThumbnail(const std::string * rawJSON) {
		std::vector<std::string> values = json::getValues(rawJSON->c_str(),
			{ "url", "proxy_url", "height", "width" });
		url = values[0];
		proxy_url = values[1];
		height = std::stoi(values[2]);
		width = std::stoi(values[3]);
	}

	EmbedProvider::EmbedProvider() {
	}

	EmbedProvider::EmbedProvider(const std::string * rawJSON) {
		std::vector<std::string> values = json::getValues(rawJSON->c_str(),
			{ "name", "url" });
		name = values[0];
		url = values[1];
	}
}