#include "attachment.h"

namespace SleepyDiscord {
	Attachment::Attachment() {
	}

	Attachment::~Attachment() {
	}
	
	Attachment::Attachment(const std::string * rawJSON) : Attachment(json::getValues(rawJSON->c_str(), fields)) {}

	Attachment::Attachment(const std::vector<std::string> values) :
		//variable           condition  modifier    value                     felid
		Parent    (                                 values[index(fields, "id"       )]    ),
		filename  (                                 values[index(fields, "filename" )]    ),
		size      ( std::stoull(                    values[index(fields, "size"     )]   )),
		url       (                                 values[index(fields, "url"      )]    ),
		proxy_url (                                 values[index(fields, "proxy_url")]    ),
		height    (modIfElse(isDefined, std::stoul, values[index(fields, "height"   )], 0)),
		width     (modIfElse(isDefined, std::stoul, values[index(fields, "width"    )], 0))
	{}
	const std::initializer_list<const char*const> Attachment::fields = {
		"id", "filename", "size", "url", "proxy_url", "height", "width"
	};
}