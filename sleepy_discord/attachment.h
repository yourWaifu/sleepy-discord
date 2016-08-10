#pragma once
#include <string>
#include "discord_object_interface.h"

namespace SleepyDiscord {
	struct Attachment : public DiscordObject {
	public:
		Attachment();
		~Attachment();
		void fillOut(JSON_object * _JSON_object);
		void fillOut(const char* name, void * value);
		std::string id;
		std::string filename;
		double* size;	//all json numbers are doubles
		std::string url;
		std::string proxy_url;
		double* height;	//nullable
		double* width;	//nullable
	};
}
