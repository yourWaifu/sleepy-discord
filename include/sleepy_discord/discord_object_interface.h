#pragma once
#include <list>
#include "json_wrapper.h"
#include "snowflake.h"
#include "error.h"
#include "http.h"

namespace SleepyDiscord {
	class BaseDiscordClient;

	class DiscordObject {

	};

	struct EmptyDiscordObject {
		EmptyDiscordObject() = default;
		EmptyDiscordObject(const nonstd::string_view& rawJSON) :
			EmptyDiscordObject(json::fromJSON<EmptyDiscordObject>(rawJSON)) {}
		EmptyDiscordObject(const json::Value & json) :
			EmptyDiscordObject(json::fromJSON<EmptyDiscordObject>(json)) {}

		JSONStructStart
			std::make_tuple();
		JSONStructEnd
	};

	template <class Derived>
	class IdentifiableDiscordObject : public DiscordObject {
	public:
		IdentifiableDiscordObject() = default;
		IdentifiableDiscordObject(Snowflake<Derived> id) : ID(id) {}

		using Parent = IdentifiableDiscordObject<Derived>;
		using Identifier = Snowflake<Derived>;

		Snowflake<Derived> ID;
		
		inline operator Snowflake<Derived>&() {
			return ID;
		}

		inline bool empty() const {
			return ID.empty();
		}

		void merge(Derived& changes) {
			json::mergeObj(*this, changes);
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