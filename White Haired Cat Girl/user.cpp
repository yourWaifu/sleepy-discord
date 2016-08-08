#include "user.h"

namespace SleepyDiscord {
	User::~User() {
	}

	User::User() {
	}

	void User::fillOut(JSON_object * _JSON_object) {
		DiscordObject::fillOut(_JSON_object);
	}

	void User::fillOut(const char* name, void * value) {
		switch (name[0]) {
		case 'u': username = (char*)value; break;
		case 'i': id = (char*)value; break;
		case 'd': discriminator = (char*)value; break;
		case 'a': avatar = (char*)value; break;
		case 'b': bot = *(bool*)value; break;
		case 'v': verified = *(bool*)value; break;
		case 'e': avatar = (char*)value; break;
		default: break;
		}
	}
}