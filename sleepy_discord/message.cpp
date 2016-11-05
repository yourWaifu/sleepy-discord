#include "message.h"
#include "experimental.h"
namespace SleepyDiscord {
	Message::Message() {

	}

	Message::~Message() {
		if (0 < numOfEmbeds) delete[] embeds;
		if (0 < numOfAttachments) delete[] attachments;
	}

	Message::Message(JSON * jsonMessage) {
		DiscordObject::fillOut(jsonMessage);
	}

	Message::Message(JSON_object * messageJSONObject) {
		DiscordObject::fillOut(messageJSONObject);
	}

	Message::Message(const std::string * rawJson, const unsigned int startPosition) 
//#ifdef EXPERIMENTAL
		//: DiscordObject(), startPosition(startPosition) , rawOriginJSON(rawJson)	
//#endif // EXPERIMENTAL
	{
		//set numbers to defaults
		numOfAttachments = 0;
		numOfEmbeds = 0;
		nonce = 0;
		const char* names[] = { "id", "channel_id", "content", "timestamp" , "edited_timestamp", "tts",
		                        "mention_everyone", "author", "mentions", "mention_roles", "attachments",
		                        "embeds", "nonce", "pinned" };
		const unsigned int arraySize = sizeof(names) / sizeof(*names);
		std::string values[arraySize];
		JSON_getValues(rawJson->c_str(), names, values, arraySize);
		id = values[0];
		channel_id = values[1];
		content = values[2];
		timestamp = values[3];
		edited_timestamp = values[4];
		tts = values[5][0] == 't' ? true : false;
		mention_everyone = values[6][0] == 't' ? true : false;
		author = User(values + 7);
		JSON_getArray<User>(values + 8, &mentions, [](User* value, std::string string) {*value = User(&string);});
		JSON_getArray<std::string>(values + 9, &mention_roles, [](std::string* value, std::string string) {*value = string;});
		//attachments
		//embeds
		if (values[12][0] != 'n') nonce = std::stoll(values[12]);
		pinned = values[13][0] == 't' ? true : false;
	}

	//void Message::fillOut(JSON_object * _JSON_object) {
	//	DiscordObject::fillOut(_JSON_object);
	//}

	void Message::fillOut(const char* name, void * value) {
		switch (name[0]) {
		case 't':
			switch (name[1]) {
			case 'i': timestamp = (char*)value; break;
			case 't': tts = *(bool*)value; break;
			case 'y': type = *(double*)value; break;
			default: break;
			} break;
		case 'p': pinned = *(bool*)value; break;
		case 'n':
			nonce = 0; //this function is going to be replaced so lets forget about it

			break;
		case 'm':
			switch (name[8]) {
			case 0: //mentions
				//mentions = fillOutArray<User>(value, numOfMentions); break;
			case 'r':
				//mention_roles = fillOutArray<std::string>(value, numOfMention_roles,
				//	[] (std::string* arrayValue, JSON_array* _array, unsigned int index) {
				//		*arrayValue = (char*)JSON_accessArray(_array, index);
				//	});
				break;
			case 'e': mention_everyone = *(bool*)value; break;
			default: break;
			} break;
		case 'i': id = (char*)value; break;
			case 'e':
				switch (name[1]) {
				case 'm': embeds = fillOutArray<Embed>(value, numOfEmbeds);
				case 'd': edited_timestamp = (char*)value; break;
				default: break;
				}
		case 'c':
			switch (name[1]) {
			case 'o': content = (char*)value; break;
			case 'h': channel_id = (char*)value; break;
			} break;
		case 'a':
			switch (name[1]) {
			case 'u': author.fillOut((JSON_object*)value); break;
			case 't': attachments = fillOutArray<Attachment>(value, numOfAttachments); break;
			default: break;
			} break;
		default: break;
		}
	}

	bool Message::startsWith(char* test) {
		return strncmp(content.c_str(), test, strlen(test)) == 0;
	}

	int Message::length() {
		return content.length();	//if this doesn't work there's a stringLength function in common.h
	}

	bool Message::isMentioned(const std::string id) {
		unsigned int size = mentions.size();
		for (unsigned int i = 0; i < size; i++)
			if (mentions[i].id  == id) return true;
		return false;
	}

	bool Message::isMentioned(User& _user) {
		return isMentioned(_user.id);
	}
}