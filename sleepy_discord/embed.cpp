#include "embed.h"

namespace SleepyDiscord {
	Embed::Embed() {
	}
	
	Embed::Embed(const std::string * rawJSON) : Embed(json::getValues(rawJSON->c_str(), fields)) {}

	Embed::Embed(const std::vector<std::string> values) :
		//variable            condition    modifier         value                     felid            else
		title      (                                        values[index(fields, "title"      )]                   ),
		type       (                                        values[index(fields, "type"       )]                   ),
		description(                                        values[index(fields, "description")]                   ),
		url        (                                        values[index(fields, "url"        )]                   ),
		thumbnail  (modIfElse(isSpecified, EmbedThumbnail, &values[index(fields, "thumbnail"  )], EmbedThumbnail())),
		provider   (modIfElse(isSpecified, EmbedProvider , &values[index(fields, "provider"   )], EmbedProvider ()))
	{}
	
	const std::initializer_list<const char*const> Embed::fields = {
		"title", "type", "description", "url", "thumbnail", "provider"
	};

	EmbedThumbnail::EmbedThumbnail() {
	}

	EmbedThumbnail::EmbedThumbnail(const std::string * rawJSON) : EmbedThumbnail(json::getValues(rawJSON->c_str(), fields)) {}

	EmbedThumbnail::EmbedThumbnail(const std::vector<std::string> values) :
		//variable modifier  value                     felid
		url       (          values[index(fields, "url"      )] ),
		proxy_url (          values[index(fields, "proxy_url")] ),
		height    (std::stoi(values[index(fields, "height"   )])),
		width     (std::stoi(values[index(fields, "width"    )]))
	{}
	
	const std::initializer_list<const char*const> EmbedThumbnail::fields = {
		"url", "proxy_url", "height", "width"
	};

	EmbedProvider::EmbedProvider() {
	}

	EmbedProvider::EmbedProvider(const std::string * rawJSON) : EmbedProvider(json::getValues(rawJSON->c_str(), fields)) {}

	EmbedProvider::EmbedProvider(const std::vector<std::string> values) :
		//variable value               felid
		name(values[index(fields, "name")]),
		url (values[index(fields, "url" )])
	{}

	const std::initializer_list<const char*const> EmbedProvider::fields = {
		"name", "url"
	};
}