#include "json_wrapper.h"
#include <stdexcept>

namespace SleepyDiscord { namespace json {
	const std::string createJSON(std::initializer_list<std::pair<std::string, std::string>> json) {
		std::string target;
		target.reserve(2);	//revents crash
		for (std::pair<std::string, std::string> pair : json) {
			if (pair.second != "") {
				target += ",\"" + pair.first + "\":" + pair.second;
			}
		}
		target[0] = '{';
		target.push_back('}');
		return target;
	}

	const std::string string(const std::string s) {
		return s != "" ? '\"' + s + '\"' : "";
	}

	const std::string UInteger(const uint64_t num) {
		return std::to_string(num & 0x3FFFFFFFFFFFFF);   //just in case numbers are larger then 53 bits
	}

	const std::string optionalUInteger(const uint64_t num) {
		return num ? UInteger(num) : "";
	}

	const std::string integer(const int64_t num) {
		return std::to_string(num & 0x803FFFFFFFFFFFFF);  //just in case numbers are larger then 53 bits
	}

	const std::string optionalInteger(const int64_t num) {
		return num ? integer(num) : "";
	}

	const std::string boolean(const bool boolean) {
		return boolean ? "true" : "false";
	}
}}