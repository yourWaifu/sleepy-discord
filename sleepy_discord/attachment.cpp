#include "Attachment.h"

namespace SleepyDiscord {
	Attachment::Attachment() {
	}

	Attachment::~Attachment() {
	}
	
	void Attachment::fillOut(JSON_object * _JSON_object) {
		DiscordObject::fillOut(_JSON_object);
	}

	void Attachment::fillOut(const char * name, void * value) {
		switch (name[0]) {
		case 'i': id = (char*)value; break;
		case 'f': filename = (char*)value; break;
		case 's': size = (double*)value; break;
		case 'u': url = (char*)value; break;
		case 'p': proxy_url = (char*)value; break;
		case 'h': height = (double*)value; break;
		case 'w': width = (double*)value; break;
		default: break;
		}
	}
}