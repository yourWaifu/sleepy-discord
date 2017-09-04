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

		template<typename Number, typename Function>
		inline void implementToNumber(std::string& value, Number& number, Function convertFunction) {
			number = isDefined(value) ? convertFunction(value) : 0;
		}

		inline void toNumber(int& number, std::string& value) {
			implementToNumber(value, number, [](std::string& v)->int {
				return std::stoi(v);
			});
		}

		inline void toNumber(unsigned long& number, std::string& value) {
			implementToNumber(value, number, [](std::string& v)->unsigned long {
				return std::stoul(v);
			});
		}
	};

	//constexpr unsigned int index(std::initializer_list<const char *const> names, const char * name, unsigned int i = 0) {
	//	for (const char *const n : names)
	//		if (strcmp(n, name) != 0) ++i;
	//		else break;
	//	return i;
	//}//sadly this doesn't work on c++11

	constexpr unsigned int index(std::initializer_list<const char *const> names, const char * name, unsigned int i = 0) {
		return i + names.begin() != names.end() && strcmp(*(i + names.begin()), name) != 0 ? index(names, name, i + 1) : i;
	}

	template <class _DiscordObject>
	std::vector<_DiscordObject> JSON_getArray(const std::string* _source) {
		return json::RawJSONArrayWrapper<_DiscordObject>(*_source);
	}
}