#include "server.h"

namespace SleepyDiscord {
	Server::Server(JSON* jsonMessage) {
		DiscordObject::fillOut(jsonMessage);
	}

	Server::~Server() {

	}

	void Server::fillOut(JSON_object * _JSON_object) {
		DiscordObject::fillOut(_JSON_object);
	}

	void Server::fillOut(const char * name, void * value) {
		switch (name[0]) {
		case 'a':
			switch (name[4]) {
			case 'c': afk_channel_id = (char*)value; break;
			case 't': afk_timeout = (double*)value; break;
			default: break;
			} break;
		case 'd': default_message_notifications = (double*)value; break;
		case 'e':
			switch (name[6]) {
				case 'e': embed_enable = (bool*)value; break;
				case 'c': embed_channel_id = (char*)value; break;
				default: break;
			} break;
		case 'i':
			switch (name[1]) {
			case 'd': id = (char*)value;
			case 'c': icon = (char*)value;
			default: break;
			} break;
		case 'j': joined_at = (char*)value; break;
		case 'l': large = (bool*)value; break;
		case 'm': mfa_level = (double*)value; break;
		case 'n': this->name = (char*)value; break;
		case 'o': owner_id = (char*)value; break;
		case 'r': region = (char*)value; break;
		case 's': splash = (char*)value; break;
		case 'u': unavailable = (bool*)value; break;
		case 'v': verfication_level = (double*)value; break;
		default: break;
		}
	}

}