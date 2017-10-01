#include "attachment.h"

namespace SleepyDiscord {
	Attachment::Attachment() {
	}

	Attachment::~Attachment() {
	}
	
	Attachment::Attachment(const std::string * rawJSON) {
		//parse json and convert from string to type
		std::initializer_list<const char*const> names = {
			"id", "filename", "size", "url", "proxy_url", "height", "width"
		};
		std::vector<std::string> values = json::getValues(rawJSON->c_str(), names);
		id        =                    values[index(names, "id"       )] ;
		filename  =                    values[index(names, "filename" )] ;
		size      = std::stoull(       values[index(names, "size"     )]);
		url       =                    values[index(names, "url"      )] ;
		proxy_url =                    values[index(names, "proxy_url")] ;
		const std::string heightTemp = values[index(names, "height"   )] ;
		height    = isDefined(heightTemp) ? std::stoul(heightTemp) : 0;
		const std::string widthTemp  = values[index(names, "width"    )] ;
		width     = isDefined(widthTemp ) ? std::stoul(widthTemp ) : 0;
	}
}