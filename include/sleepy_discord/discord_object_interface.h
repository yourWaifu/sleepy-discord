#pragma once
#include <list>
#include "json_wrapper.h"
#include "snowflake.h"
#include "error.h"
#include "http.h"

namespace SleepyDiscord {
	class BaseDiscordClient;

	class DiscordObject {
//	public:
//		DiscordObject() {}
//
//	protected:
//		//functions for parsing JSON
//		inline bool getBool(const json::Value& string) {
//			return string[0] == 't';
//		}
//		
//		//for optional fields
//		inline bool isSpecified(const json::Value& string) {
//			return string.length() != 0;
//		}
//
//		//for nullable and opinional fields
//		inline bool isDefined(const json::Value& string) {
//			return isSpecified(string) && string != "null";
//		}
//
//		//for nullable fields
//		inline bool isDefault(const json::Value& string) {
//			return isSpecified(string) && string == "null";
//		}
//		
//		//pointer to referance convertion, needed or you will get an error
//		inline bool isSpecified(const json::Value* string) {
//			return isSpecified(*string);
//		}
//
//		inline bool isDefined(const json::Value* string) {
//			return isDefined(*string);
//		}
//
//		inline bool isDefault(const json::Value* string) {
//			return isDefault(*string);
//		}
//
//		//some of those function declarations got so long that
//		//I gave them muitiple lines
/*#define \
		modIf(condition, variable, modifier, value) \
			if (condition(value)) variable = modifier(value)*/
#define \
		modIfElse(condition, modifier, value, el) \
			condition(value) ? modifier(value) : el
//
//		////this doesn't work, but the above does
//		//template<typename Type>
//		//void modIf(
//		//	bool (*condition)(const std::string&),
//		//	Type& variable, 
//		//	Type (*function)(const std::string& _source),
//		//	std::string& value
//		//) {
//		//	if (condition(value)) variable = function(value);
//		//}
//
/*#define \
		setIf(condition, variable, value) \
			if (condition(value)) variable = value*/
#define \
		setIfElse(condition, value, el) \
			condition(value) ? value : el
//
//		template<typename Number>
//		inline Number toNumber(
//			Number (*convertFunction)(const std::string&, size_t*, int),
//			const json::Value& value
//			) {
//			return isDefined(value) ? convertFunction(value.to_string(), 0, 10) : 0;
//		}
//
//		inline int toInt(const json::Value& value) {
//			return toNumber(std::stoi, value);
//		}
//
//		inline long long toLongLong(const json::Value& value) {
//			return toNumber(std::stoll, value);
//		}
//
//		inline unsigned long toUnsignedLong(const json::Value& value) {
//			return toNumber(std::stoul, value);
//		}
	};

	template <class Derived>
	class IdentifiableDiscordObject : public DiscordObject {
	public:
		IdentifiableDiscordObject() = default;
		IdentifiableDiscordObject(Snowflake<Derived> id) : ID(id) {}

		using Parent = IdentifiableDiscordObject<Derived>;

		Snowflake<Derived> ID;
		
		inline operator Snowflake<Derived>&() {
			return ID;
		}

		inline bool empty() const {
			return ID.empty();
		}

		template<class DiscordObject>
		inline bool operator==(const Snowflake<DiscordObject>& right) const {
			return ID == static_cast<Snowflake<DiscordObject>>(right);
		}
		
		template<class DiscordObject>
		inline bool operator!=(const Snowflake<DiscordObject>& right) const {
			return ID != static_cast<Snowflake<DiscordObject>>(right);
		}

		inline bool operator==(const Snowflake<Derived>& right) const {
			return operator==<Derived>(right);
		}

		inline bool operator!=(const Snowflake<Derived>& right) const {
			return operator!=<Derived>(right);
		}

		inline bool operator==(const IdentifiableDiscordObject<Derived>& right) const {
			return ID == right.ID;
		}

		inline bool operator!=(const IdentifiableDiscordObject<Derived>& right) const {
			return ID != right.ID;
		}

		inline const Time getTimestamp() {
			return ID.timestamp();
		}
	};

	//constexpr unsigned int index(std::initializer_list<const char *const> names, const char * name, unsigned int i = 0) {
	//	for (const char *const n : names)
	//		if (strcmp(n, name) != 0) ++i;
	//		else break;
	//	return i;
	//}//sadly this doesn't work on c++11, leaving this here for the future

	template <template<class...> class Container, typename Type> //forces this be done at compile time, I think, and hope it does
	constexpr unsigned int index(Container<Type*const> names, Type * name, unsigned int i = 0) {
		return i + names.begin() != names.end() && strcmp(*(i + names.begin()), name) != 0 ? index(names, name, i + 1) : i;
	}

	template <class _DiscordObject>
	std::vector<_DiscordObject> JSON_getArray(const std::string* _source) {
		return json::ArrayWrapper<_DiscordObject>(*_source);
	}

	//somethings I need it to be a reference
	template <class _DiscordObject, class Type>
	inline std::vector<_DiscordObject> JSON_getArray(const Type& _source) {
		return json::ArrayWrapper<_DiscordObject>(_source);
	}

	template <class _DiscordObject, class Type>
	inline std::list<_DiscordObject> JSON_getList(const Type& _source) {
		return json::ArrayWrapper<_DiscordObject>(_source);
	}
}