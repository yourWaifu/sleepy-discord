#pragma once
#include <string>
#include <chrono>
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
#include <stdexcept>
#endif

namespace SleepyDiscord {
	//Stops you from mixing up different types of ids, like using a message_id as a user_id
	template <typename DiscordObject>
	struct Snowflake {
		Snowflake(                                         ) : raw(""        ) {}
		Snowflake(const std::string             & snow     ) : raw( snow     ) {}
		Snowflake(const std::string             * snow     ) : raw(*snow     ) {}
		Snowflake(const char                    * snow     ) : raw( snow     ) {}
		Snowflake(const DiscordObject           & object   ) : Snowflake(object. ID) {}
		Snowflake(const DiscordObject           * object   ) : Snowflake(object->ID) {}

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

		std::chrono::time_point<std::chrono::steady_clock> timestamp() {
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
			if (raw == "") throw std::invalid_argument("invalid snow in Snowflake");
#endif
			return std::chrono::time_point<std::chrono::steady_clock>(std::chrono::milliseconds((std::stoll(raw) >> 22) + discordEpoch));
		}

	private:
		std::string raw;
		static const uint64_t discordEpoch = 1420070400000;	//the first second of 2015 since epoch
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