#include "embed.h"

namespace SleepyDiscord {
	Embed::Embed() {
	}

	Embed::Embed(const std::string * rawJSON) {
		std::initializer_list<const char*const> names = {
			"title", "type", "description", "url", "thumbnail", "provider"
		};

		std::vector<std::string> values = json::getValues(rawJSON->c_str(), names);
		title       =                     values[index(names, "title"      )];
		type        =                     values[index(names, "type"       )];
		description =                     values[index(names, "description")];
		url         =                     values[index(names, "url"        )];
		const std::string thumbnailTemp = values[index(names, "thumbnail"  )];
		if (isSpecified(thumbnailTemp)) thumbnail = EmbedThumbnail(&thumbnailTemp);	//if it doesn't have a thumbnail, it doesn't need to make one
		const std::string providerTemp  = values[index(names, "provider"   )];
		if (isSpecified(providerTemp )) provider  = EmbedProvider( &providerTemp );
	}

	EmbedThumbnail::EmbedThumbnail() {
	}

	EmbedThumbnail::EmbedThumbnail(const std::string * rawJSON) {
		std::initializer_list<const char*const> names = {
			"url", "proxy_url", "height", "width"
		};

		std::vector<std::string> values = json::getValues(rawJSON->c_str(), names);
		url       =           values[index(names, "url"      )] ;
		proxy_url =           values[index(names, "proxy_url")] ;
		height    = std::stoi(values[index(names, "height"   )]);
		width     = std::stoi(values[index(names, "width"    )]);
	}

	EmbedProvider::EmbedProvider() {
	}

	EmbedProvider::EmbedProvider(const std::string * rawJSON) {
		std::initializer_list<const char*const> names = {
			"name", "url"
		};

		std::vector<std::string> values = json::getValues(rawJSON->c_str(), names);
		name = values[index(names, "name")];
		url  = values[index(names, "url" )];
	}
}