#pragma once
#include <string>
#include "snowflake.h"
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct Attachment : public IdentifiableDiscordObject<Attachment> {
	public:
		Attachment() = default;
		//~Attachment();
		//Attachment(const std::string * rawJSON);
		Attachment(const json::Value& json);
		Attachment(const nonstd::string_view& rawJSON);
		//Attachment(const json::Values values);
		std::string filename;
		uint64_t size = 0;
		std::string url;
		std::string proxy_url;
		uint64_t height = 0;
		uint64_t width = 0;
		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&Attachment::ID       , "id"       , json::REQUIRIED_FIELD),
				json::pair(&Attachment::filename , "filename" , json::REQUIRIED_FIELD),
				json::pair(&Attachment::size     , "size"     , json::REQUIRIED_FIELD),
				json::pair(&Attachment::url      , "url"      , json::REQUIRIED_FIELD),
				json::pair(&Attachment::proxy_url, "proxy_url", json::REQUIRIED_FIELD),
				json::pair(&Attachment::height   , "height"   , json::NULLABLE_FIELD ),
				json::pair(&Attachment::width    , "width"    , json::NULLABLE_FIELD )
			);
		JSONStructEnd
	};
}
