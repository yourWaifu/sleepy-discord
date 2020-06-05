#pragma once
#include <string>
#include <chrono>
#include <algorithm>
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
#include <stdexcept>
#endif
#include "nonstd/string_view.hpp"
#include "json_wrapper.h"

namespace SleepyDiscord {
	using Time = int64_t;

	//Stops you from mixing up different types of ids, like using a message_id as a user_id
	template <typename DiscordObject>
	struct Snowflake {
		using RawType = std::string;
		
		Snowflake(                                  ) = default;
		Snowflake(const std::string         & snow  ) : raw( snow                         ) {}
		Snowflake(const std::string         * snow  ) : raw(*snow                         ) {}
		Snowflake(const char                * snow  ) : raw( snow                         ) {}
		Snowflake(const nonstd::string_view & snow  ) : raw(snow.data(), snow.length()    ) {}
		Snowflake(const Snowflake           & flake ) : Snowflake(flake.string(          )) {}
		Snowflake(const DiscordObject       & object) : Snowflake(object. ID              ) {}
		Snowflake(const DiscordObject       * object) : Snowflake(object->ID              ) {}
		Snowflake(const int64_t               number) : Snowflake(std::to_string(number  )) {}
		Snowflake(const json::Value         & value ) :
			Snowflake(value.IsString() ? json::toStdString(value) : std::string()) {}
		~Snowflake() = default;

		inline bool operator==(const Snowflake& right) const {
			return raw == right.raw;
		}

		inline bool operator!=(const Snowflake& right) const {
			return raw != right.raw;
		}

		inline bool operator==(const char* right) const {
			return raw == right;
		}

		inline bool operator!=(const char* right) const {
			return raw != right;
		}

		inline operator const std::string&() const { return raw; }

		inline const std::string& string() const { return operator const std::string&(); }
		inline const int64_t number() const { return std::stoll(raw); }

		std::chrono::time_point<std::chrono::steady_clock> timestamp() const {
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
			if (raw == "") throw std::invalid_argument("invalid snow in Snowflake");
#endif
			return std::chrono::time_point<std::chrono::steady_clock>(std::chrono::milliseconds((std::stoll(raw) >> 22) + discordEpoch));
		}

		template<class iterator>
		inline iterator findObject(iterator begin, iterator end) const {
			return std::find_if(begin, end, [&](const DiscordObject& object) {
				return operator==(static_cast<DiscordObject>(object));
			});
		}

		//Magical code from stackflow
		//https://stackoverflow.com/a/87846
		template<class Container>
		struct HasAFindFunction {
			using SuccessType = char;
			using FailureType = int;
			template<class _Container, size_t (_Container::*)() const> struct Magic {};
			template<class _Container> static SuccessType Test(Magic<_Container, &_Container::find>*);
			template<class _Container> static FailureType Test(...);
			static const bool Value = sizeof(Test<Container>(0)) == sizeof(SuccessType);
		};

		template<class Container>
		auto findObject(Container& objects, std::true_type) const -> decltype(objects.begin()) {
			return objects.find(operator const std::string&());
		}

		template<class Container>
		auto findObject(Container& objects, std::false_type) const -> decltype(objects.begin()) {
			return findObject(objects.begin(), objects.end());
		}

		template<class Container>
		auto findObject(Container& objects) const -> decltype(objects.begin()) {
			return findObject(objects, std::integral_constant<bool, HasAFindFunction<Container>::Value>());
		}

	private:
		RawType raw = {};
		static const Time discordEpoch = 1420070400000;	//the first second of 2015 since epoch
	};

	template <typename DiscordOject>
	inline std::string operator+(const char * left, Snowflake<DiscordOject>& right) {
		return left + right.operator const std::string&();
	}

	template <typename DiscordOject>
	inline bool operator==(const char * left, Snowflake<DiscordOject>& right) {
		return left == right.operator const std::string&().c_str();
	}

	template <typename DiscordOject>
	inline bool operator!=(const char * left, Snowflake<DiscordOject>& right) {
		return left != right.operator const std::string&().c_str();
	}
}