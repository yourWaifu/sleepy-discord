#include "discord_object_interface.h"

namespace SleepyDiscord {
	void DiscordObject::fillOut(JSON * _JSON) {
		fillOut((JSON_object*)JSON_access1(_JSON, "d"));
	}

	void DiscordObject::fillOut(JSON_object * _JSON_object) {
		bool loop = true;
		for (int i = 0; loop; i++) {
			loop = JSON_isPairValid(_JSON_object->pairs + i);
			if (loop) fillOut(_JSON_object->pairs[i].name, _JSON_object->pairs[i].value);
		}
	}
}