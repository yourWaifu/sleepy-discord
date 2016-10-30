#include "user.h"

namespace SleepyDiscord {
	User::~User() {
	}

	User::User() {
	}

	User::User(const std::string * rawJSON, const unsigned int startPosition) {
		const char* names[] = { "id", "username", "discriminator", "avatar", "bot", "verified", "email" };
		const unsigned int arraySize = sizeof(names) / sizeof(*names);
		std::string values[arraySize];
		JSON_getValues(rawJSON->c_str(), names, values, arraySize);
		id = values[0];
		username = values[1];
		discriminator = values[2];
		avatar = values[3];
		bot = values[4][0] == 't' ? true : false;
		verified = values[5][0] == 't' ? true : false;
		email = values[6];
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
		case 'e': email = (char*)value; break;
		default: break;
		}
	}
}