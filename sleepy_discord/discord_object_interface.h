#pragma once
#include "json.h"

namespace SleepyDiscord {
	class BaseDiscordClient;

	class DiscordObject {
	public:
		DiscordObject() {}
		//DiscordObject(BaseDiscordClient* client);	//I'm deciding weather I should add this or not
		//void changeDiscordClient(BaseDiscordClient& client);

	protected:
		//BaseDiscordClient* client;   is this a good idea?

		//functions for parsing JSON
		inline bool getBool(const std::string& string) {
			return string[0] == 't';
		}
		
		inline bool isSpecified(const std::string& string) {
			return string[0] != 0;
		}

		inline bool isDefined(const std::string& string) {
			return isSpecified(string) && string != "null";
		}

		inline bool isDefault(const std::string& string) {	
			return isSpecified(string) || string == "null";
		}
	};

	constexpr unsigned int index(std::initializer_list<const char *const> names, const char * name, unsigned int i = 0) {
		for (const char *const n : names)
			if (strcmp(n, name) != 0) ++i;
			else break;
			return i;
	}



	template <class _DiscordObject>
	std::vector<_DiscordObject> JSON_getArray(const std::string* _source) {
		return json::getArray<_DiscordObject>(_source, [](_DiscordObject* value, std::string string) {*value = _DiscordObject(&string); });
	}
}