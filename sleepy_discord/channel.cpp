#include "channel.h"

namespace SleepyDiscord {
	Channel::~Channel() {

	}

	void Channel::fillOut(const char * _name, void * value) {
		switch (_name[0]) {
		case 'i': switch (_name[1]) {
			case 'd': id = (char*)value; break;
			case 's': is_private = (bool*)value; break;
		} break;
		case 'g': guild_id = (char*)value; break;
		case 'n': name = (char*)value; break;
		case 't': switch (_name[1]) {
			case 'y': type = (char*)value; break;
			case 'o': topic = (char*)value; break;
		} break;
		case 'p': position = (double*)value; break;
		case 'l': lastMessage_id = (char*)value; break;
		case 'b': bitrate = (double*)value; break;
		case 'u': userLimit = (double*)value; break;
		default: break;
		}
	}

	DirectMessageChannel::~DirectMessageChannel() {

	}

	void DirectMessageChannel::fillOut(const char* name, void * value) {
		switch (name[0]) {
		case 'i': switch (name[1]) {
			case 'd': id = (char*)value; break;
			case 's': is_private = (bool*)value; break;
		} break;
		case 'r': recipient.fillOut((JSON_object*)value); break;
		case 'l': lastMessage_id = (char*)value; break;
		default: break;
		}
	}
}