#include "embed.h"

namespace SleepyDiscord {
	//Embed::Embed() {
	//}
	
	//Embed::Embed(const std::string * rawJSON) : Embed(json::getValues(rawJSON->c_str(), fields)) {}

	//Embed::Embed(const nonstd::string_view& rawJSON) : Embed(json::getValues(rawJSON.data(), fields)) {}

	//Embed::Embed(const json::Values values) :
	//	//variable            condition    modifier        value                     felid            else
	//	title      (                                       values[index(fields, "title"      )]                   ),
	//	type       (                                       values[index(fields, "type"       )]                   ),
	//	description(                                       values[index(fields, "description")]                   ),
	//	url        (                                       values[index(fields, "url"        )]                   ),
	//	thumbnail  (modIfElse(isSpecified, EmbedThumbnail, values[index(fields, "thumbnail"  )], EmbedThumbnail())),
	//	provider   (modIfElse(isSpecified, EmbedProvider , values[index(fields, "provider"   )], EmbedProvider ()))
	//{}
	
	//const std::initializer_list<const char*const> Embed::fields = {
	//	"title", "type", "description", "url", "thumbnail", "provider"
	//};

	//EmbedThumbnail::EmbedThumbnail() : height(0), width(0)
	//{}

	//EmbedThumbnail::EmbedThumbnail(const std::string * rawJSON) : EmbedThumbnail(json::getValues(rawJSON->c_str(), fields)) {}

	//EmbedThumbnail::EmbedThumbnail(const nonstd::string_view& rawJSON) : EmbedThumbnail(json::getValues(rawJSON.data(), fields)) {}

	//EmbedThumbnail::EmbedThumbnail(const json::Values values) :
	//	//variable modifier  value                     felid
	//	url       (          values[index(fields, "url"      )]             ),
	//	proxy_url (          values[index(fields, "proxy_url")]             ),
	//	height    (std::stoi(values[index(fields, "height"   )].to_string())),
	//	width     (std::stoi(values[index(fields, "width"    )].to_string()))
	//{}
	//
	//const std::initializer_list<const char*const> EmbedThumbnail::fields = {
	//	"url", "proxy_url", "height", "width"
	//};

	//EmbedProvider::EmbedProvider() {
	//}

	//EmbedProvider::EmbedProvider(const std::string * rawJSON) : EmbedProvider(json::getValues(rawJSON->c_str(), fields)) {}
	//
	//EmbedProvider::EmbedProvider(const nonstd::string_view& rawJSON) : EmbedProvider(json::getValues(rawJSON.data(), fields)) {}

	//EmbedProvider::EmbedProvider(const json::Values values) :
	//	//variable value               felid
	//	name(values[index(fields, "name")]),
	//	url (values[index(fields, "url" )])
	//{}

	//const std::initializer_list<const char*const> EmbedProvider::fields = {
	//	"name", "url"
	//};

	EmbedThumbnail::EmbedThumbnail(const json::Value & json) :
		EmbedThumbnail(json::fromJSON<EmbedThumbnail>(json)) {
	}
	EmbedThumbnail::EmbedThumbnail(const nonstd::string_view & json) :
		EmbedThumbnail(json::fromJSON<EmbedThumbnail>(json)) {
	}

	EmbedVideo::EmbedVideo(const json::Value & json) :
		EmbedVideo(json::fromJSON<EmbedVideo>(json)) {
	}
	EmbedVideo::EmbedVideo(const nonstd::string_view & json) :
		EmbedVideo(json::fromJSON<EmbedVideo>(json)) {
	}

	EmbedImage::EmbedImage(const json::Value & json) :
		EmbedImage(json::fromJSON<EmbedImage>(json)) {
	}
	EmbedImage::EmbedImage(const nonstd::string_view & json) :
		EmbedImage(json::fromJSON<EmbedImage>(json)) {
	}

	EmbedProvider::EmbedProvider(const json::Value & json) :
		EmbedProvider(json::fromJSON<EmbedProvider>(json)) {
	}
	EmbedProvider::EmbedProvider(const nonstd::string_view & json) :
		EmbedProvider(json::fromJSON<EmbedProvider>(json)) {
	}

	EmbedAuthor::EmbedAuthor(const json::Value & json) :
		EmbedAuthor(json::fromJSON<EmbedAuthor>(json)) {
	}
	EmbedAuthor::EmbedAuthor(const nonstd::string_view & json) :
		EmbedAuthor(json::fromJSON<EmbedAuthor>(json)) {
	}

	EmbedFooter::EmbedFooter(const json::Value & json) :
		EmbedFooter(json::fromJSON<EmbedFooter>(json)) {
	}
	EmbedFooter::EmbedFooter(const nonstd::string_view & json) :
		EmbedFooter(json::fromJSON<EmbedFooter>(json)) {
	}

	EmbedField::EmbedField(const json::Value & json) :
		EmbedField(json::fromJSON<EmbedField>(json)) {
	}
	EmbedField::EmbedField(const nonstd::string_view & json) :
		EmbedField(json::fromJSON<EmbedField>(json)) {
	}

	Embed::Embed(const json::Value & json) :
		Embed(json::fromJSON<Embed>(json)) {
	}
	Embed::Embed(const nonstd::string_view & json) :
		Embed(json::fromJSON<Embed>(json)) {
	}
}