#include "discord_object_interface.h"

namespace SleepyDiscord {
	void DiscordObject::fillOut(JSON * _JSON) { //I have a feeling that it would a good idea to remove this
		JSON_object * d_object = (JSON_object*)JSON_access1(_JSON, "d");
		if(d_object) fillOut(d_object);
		else fillOut(_JSON->object);
	}

	void DiscordObject::fillOut(JSON_object * _JSON_object) {
		bool loop = true;
		for (int i = 0; loop; i++) {
			loop = JSON_isPairValid(_JSON_object->pairs + i);
			if (loop) fillOut(_JSON_object->pairs[i].name, _JSON_object->pairs[i].value);
		}
	}
}