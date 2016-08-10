#include "embed.h"

namespace SleepyDiscord {
	void Embed::fillOut(JSON_object* _JSON_object) {
		DiscordObject::fillOut(_JSON_object);
	}

	void Embed::fillOut(const char* name, void* value) {
		switch (name[0]) {
		case 't': switch (name[1]) {
			case 'i': title = (char*)value; break;
			case 'y': type = (char*)value; break;
			case 'h': thumbnail.fillOut((JSON_object*)value); break;
			default: break;
		} break;
		case 'd': description = (char*)value; break;
		case 'u': url = (char*)value; break;
        case 'p': provider.fillOut((JSON_object*)value); break;
		default: break;
		}
    }

    void EmbedThumbnail::fillOut(JSON_object* _JSON_object) {
		DiscordObject::fillOut(_JSON_object);
	}

    void EmbedThumbnail::fillOut(const char* name, void* value) {
		switch (name[0]) {
			case 'u': url = (char*)value; break;
			case 'p': proxy_url = (char*)value; break;
			case 'h': height = (double*)value; break;
			case 'w': width = (double*)value; break;
			default: break;
		}
    }

    void EmbedProvider::fillOut(JSON_object* _JSON_object) {
		DiscordObject::fillOut(_JSON_object);
	}

    void EmbedProvider::fillOut(const char* name, void* value) {
		switch (name[0]) {
			case 'n': this->name = (char*)value; break;
			case 'u': url = (char*)value; break;
			default: break;
		}
    }
}