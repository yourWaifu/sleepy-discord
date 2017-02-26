#include "Attachment.h"

namespace SleepyDiscord {
	Attachment::Attachment() {
	}

	Attachment::~Attachment() {
	}
	
	Attachment::Attachment(const std::string * rawJSON) {
		//set numbers to defaults
		height = 0;
		width = 0;
		//parse json and convert from string to type
		const char* names[] = { "id", "filename", "size", "url", "proxy_url", "height", "width" };
		const unsigned int arraySize = sizeof(names) / sizeof(*names);
		std::string values[arraySize];
		json::getValues(rawJSON->c_str(), names, values, arraySize);
		id = values[0];
		filename = values[1];
		size = std::stoull(values[2]);
		url = values[3];
		proxy_url = values[4];
		if (values[5][0] != 'n') height = std::stoul(values[5]);
		if (values[6][0] != 'n') width = std::stoul(values[6]);
	}
}