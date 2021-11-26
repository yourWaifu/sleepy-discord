#pragma once
#include <string>
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct SendMessageParams;

	struct EmbedThumbnail : public DiscordObject {
	public:
		EmbedThumbnail() = default;
		//EmbedThumbnail(const std::string * rawJSON);
		EmbedThumbnail(const json::Value & json);
		EmbedThumbnail(const nonstd::string_view & json) :
			EmbedThumbnail(json::fromJSON<EmbedThumbnail>(json)) {
		}
		//EmbedThumbnail(const json::Values values);
		std::string url;
		std::string proxyUrl;
		int64_t height = 0;
		int64_t width = 0;

		bool empty() const {
			return url.empty();
		}

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&EmbedThumbnail::url     , "url"      , json::OPTIONAL_FIELD),
				json::pair(&EmbedThumbnail::proxyUrl, "proxy_url", json::OPTIONAL_FIELD),
				json::pair(&EmbedThumbnail::height  , "height"   , json::OPTIONAL_FIELD),
				json::pair(&EmbedThumbnail::width   , "width"    , json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	struct EmbedVideo : public DiscordObject {
	public:
		EmbedVideo() = default;
		EmbedVideo(const json::Value & json);
		EmbedVideo(const nonstd::string_view & json) :
			EmbedVideo(json::fromJSON<EmbedVideo>(json)) {
		}
		std::string url;
		int64_t height = 0;
		int64_t width = 0;

		bool empty() const {
			return url.empty();
		}

		JSONStructStart
			std::make_tuple(
				json::pair(&EmbedVideo::url   , "url"   , json::OPTIONAL_FIELD),
				json::pair(&EmbedVideo::height, "height", json::OPTIONAL_FIELD),
				json::pair(&EmbedVideo::width , "width" , json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};


	struct EmbedImage : public DiscordObject {
	public:
		EmbedImage() = default;
		EmbedImage(const json::Value & json);
		EmbedImage(const nonstd::string_view & json) :
			EmbedImage(json::fromJSON<EmbedImage>(json)) {
		}
		std::string url;
		std::string proxyUrl;
		int64_t height = 0;
		int64_t width = 0;

		bool empty() const {
			return url.empty() && proxyUrl.empty();
		}

		JSONStructStart
			std::make_tuple(
				json::pair(&EmbedImage::url     , "url"      , json::OPTIONAL_FIELD),
				json::pair(&EmbedImage::proxyUrl, "proxy_url", json::OPTIONAL_FIELD),
				json::pair(&EmbedImage::height  , "height"   , json::OPTIONAL_FIELD),
				json::pair(&EmbedImage::width   , "width"    , json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	struct EmbedProvider : public DiscordObject {
	public:
		EmbedProvider() = default;
		//EmbedProvider(const std::string * rawJSON);
		EmbedProvider(const json::Value & json);
		EmbedProvider(const nonstd::string_view & json) :
			EmbedProvider(json::fromJSON<EmbedProvider>(json)) {
		}
		//EmbedProvider(const json::Values values);
		std::string name;
		std::string url;

		bool empty() const {
			return name.empty() && url.empty();
		}

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&EmbedProvider::name, "name", json::OPTIONAL_FIELD),
				json::pair(&EmbedProvider::url , "url" , json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	struct EmbedAuthor : public DiscordObject {
	public:
		EmbedAuthor() = default;
		EmbedAuthor(const json::Value & json);
		EmbedAuthor(const nonstd::string_view & json) :
			EmbedAuthor(json::fromJSON<EmbedAuthor>(json)) {
		}
		std::string name;
		std::string url;
		std::string iconUrl;
		std::string proxyIconUrl;

		bool empty() const {
			return name.empty() && url.empty() && iconUrl.empty() && proxyIconUrl.empty();
		}

		JSONStructStart
			std::make_tuple(
				json::pair(&EmbedAuthor::name        , "name"          , json::OPTIONAL_FIELD),
				json::pair(&EmbedAuthor::url         , "url"           , json::OPTIONAL_FIELD),
				json::pair(&EmbedAuthor::iconUrl     , "icon_url"      , json::OPTIONAL_FIELD),
				json::pair(&EmbedAuthor::proxyIconUrl, "proxy_icon_url", json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	struct EmbedFooter : public DiscordObject {
	public:
		EmbedFooter() = default;
		EmbedFooter(const json::Value & json);
		EmbedFooter(const nonstd::string_view & json) :
			EmbedFooter(json::fromJSON<EmbedFooter>(json)) {
		}
		std::string text;
		std::string iconUrl;
		std::string proxyIconUrl;

		bool empty() const {
			return text.empty();
		}

		JSONStructStart
			std::make_tuple(
				json::pair(&EmbedFooter::text        , "text"          , json::REQUIRIED_FIELD),
				json::pair(&EmbedFooter::iconUrl     , "icon_url"      , json::OPTIONAL_FIELD ),
				json::pair(&EmbedFooter::proxyIconUrl, "proxy_icon_url", json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

	struct EmbedField : public DiscordObject {
	public:
		EmbedField() = default;
		EmbedField(const json::Value & json);
		EmbedField(const nonstd::string_view & json) :
			EmbedField(json::fromJSON<EmbedField>(json)) {}
		EmbedField(std::string _name, std::string _value, bool _isInline = false)
			: name(_name), value(_value), isInline(_isInline) {}
		std::string name = "";
		std::string value = "";
		bool isInline = false;

		bool empty() const {
			return value.empty();
		}

		JSONStructStart
			std::make_tuple(
				json::pair(&EmbedField::name    , "name"  , json::OPTIONAL_FIELD),
				json::pair(&EmbedField::value   , "value" , json::OPTIONAL_FIELD),
				json::pair(&EmbedField::isInline, "inline", json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	struct Embed : public DiscordObject {
	public:
		Embed() = default;
		//Embed(const std::string * rawJSON);
		Embed(const json::Value & json);
		Embed(const nonstd::string_view & json) :
			Embed(json::fromJSON<Embed>(json)) {}
		//Embed(const json::Values values);
		enum class Flag {
			INVALID_EMBED = 0,
			VALID_EMBED = 1
		};
		Embed(const Flag f) : flags(f) {}
		std::string title;
		std::string type;
		std::string description;
		std::string url;
		std::string timestamp;
		int32_t color = -1;
		EmbedFooter footer;
		EmbedImage image;
		EmbedThumbnail thumbnail;
		EmbedVideo video;
		EmbedProvider provider;
		EmbedAuthor author;
		std::vector<EmbedField> fields;

		bool empty() const {
			return (flags == Flag::INVALID_EMBED) || (title.empty() && description.empty() && url.empty() &&
				color == -1 && footer.empty() && image.empty() && thumbnail.empty() && video.empty() &&
				provider.empty() && author.empty() && fields.empty());
		}

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair                           (&Embed::title      , "title"      , json::OPTIONAL_FIELD),
				json::pair                           (&Embed::type       , "type"       , json::OPTIONAL_FIELD),
				json::pair                           (&Embed::description, "description", json::OPTIONAL_FIELD),
				json::pair                           (&Embed::url        , "url"        , json::OPTIONAL_FIELD),
				json::pair                           (&Embed::timestamp  , "timestamp"  , json::OPTIONAL_FIELD),
				json::pair<-1                       >(&Embed::color      , "color"      , json::OPTIONAL_FIELD),
				json::pair                           (&Embed::footer     , "footer"     , json::OPTIONAL_FIELD),
				json::pair                           (&Embed::image      , "image"      , json::OPTIONAL_FIELD),
				json::pair                           (&Embed::thumbnail  , "thumbnail"  , json::OPTIONAL_FIELD),
				json::pair                           (&Embed::video      , "video"      , json::OPTIONAL_FIELD),
				json::pair                           (&Embed::provider   , "provider"   , json::OPTIONAL_FIELD),
				json::pair                           (&Embed::author     , "author"     , json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&Embed::fields     , "fields"     , json::OPTIONAL_FIELD)
			);
		JSONStructEnd

	private:
		friend BaseDiscordClient;
		friend SendMessageParams;

		Flag flags = Flag::VALID_EMBED;
	};
}
