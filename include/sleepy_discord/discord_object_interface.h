#pragma once
#include "json.h"
#include "client_handle.h"

namespace SleepyDiscord {
	class BaseDiscordClient;

	class DiscordObject {
	public:
		DiscordObject() : originClientHandle(-1) {}
		DiscordObject(ClientHandle origin);
		BaseDiscordClient* getOriginClient();

	private:
		//client tracking
		ClientHandle originClientHandle;

	protected:
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

		//some of those function declarations got so long that
		//I gave them muitiple lines
#define \
		modIf(condition, variable, modifier, value) \
			if (condition(value)) variable = modifier(value)
#define \
		modIfElse(condition, variable, modifier, el, value) \
			variable = condition(value) ? modifier(value) : el

		////this doesn't work, but the above does
		//template<typename Type>
		//void modIf(
		//	bool (*condition)(const std::string&),
		//	Type& variable, 
		//	Type (*function)(const std::string& _source),
		//	std::string& value
		//) {
		//	if (condition(value)) variable = function(value);
		//}

#define \
		setIf(condition, variable, value) \
			if (condition(value)) variable = value
#define \
		setIfElse(condition, variable, el, value) \
			variable = condition(value) ? value : el

		template<typename Number>
		inline Number toNumber(
			Number (*convertFunction)(const std::string&, size_t*, int),
			std::string& value
			) {
			return isDefined(value) ? convertFunction(value, 0, 10) : 0;
		}

		inline int toInt(std::string& value) {
			return toNumber(std::stoi, value);
		}

		inline unsigned long toUnsignedLong(std::string& value) {
			return toNumber(std::stoul, value);
		}
	};

	//constexpr unsigned int index(std::initializer_list<const char *const> names, const char * name, unsigned int i = 0) {
	//	for (const char *const n : names)
	//		if (strcmp(n, name) != 0) ++i;
	//		else break;
	//	return i;
	//}//sadly this doesn't work on c++11

	template <template<class...> class Container, typename Type> //forces this be done at compile time, I think, and hope it does
	constexpr unsigned int index(Container<Type*const> names, Type * name, unsigned int i = 0) {
		return i + names.begin() != names.end() && strcmp(*(i + names.begin()), name) != 0 ? index(names, name, i + 1) : i;
	}

	template <class _DiscordObject>
	std::vector<_DiscordObject> JSON_getArray(const std::string* _source) {
		return json::ArrayWrapper<_DiscordObject>(*_source);
	}

	//somethings I need it to be a reference
	template <class _DiscordObject>
	inline std::vector<_DiscordObject> JSON_getArray(const std::string& _source) {
		return json::ArrayWrapper<_DiscordObject>(_source);
	}
}