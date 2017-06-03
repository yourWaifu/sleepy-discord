#include "json.h"
#include <stdexcept>

namespace json {
	std::vector<std::string> getValues(const char* source, std::initializer_list<const char *const> names) {
		if (*source == 0) return std::vector<std::string>{};
		const unsigned int numOfValues = names.size();
		std::vector<JSON_findMuitipleStruct>values(numOfValues);
		unsigned int ii = -1;
		for (const char *const name : names)
			values[++ii] = { name, 0, strlen(name), 0 };
		JSON_find(numOfValues, source, &values[0]);
		std::vector<std::string> targets(numOfValues);
		for (unsigned int i = 0; i < numOfValues; i++) {
			if (0 < values[i].namePosition) {
				targets[i].assign(source + values[i].namePosition, values[i].valueLength);
			}
		}
		return targets;
	}

	std::string getValue(const char* source, const char * name) {
		JSON_findMuitipleStruct value = { name, 0, strlen(name), 0 };
		JSON_find(1, source, &value);
		return std::string(source + value.namePosition, value.valueLength);
	}

	std::vector<std::string> getArray(const std::string* _source) {
		return getArray<std::string>(_source, [](std::string* value, std::string string) {*value = string;});
	}

	const std::string createJSON(std::initializer_list<std::pair<std::string, std::string>> json) {
		std::string target;
		for (auto pair : json) {
			if (pair.second != "") {
				target += ",\"" + pair.first + "\":" + pair.second;
			}
		}
		try {
			target.at(0) = '{';
			target.push_back('}');
		} catch (std::out_of_range) {
			target = "{}";
		}
		return target;
	}

	const std::string createJSONArray(std::vector<std::string> source) {
		std::string target;
		for (auto value : source) {
			if (value != "")
				target += ',' + value;
		}
		try {
			target.at(0) = '[';
			target.push_back(']');
		} catch (std::out_of_range) {
			target = "[]";
		}
		return target;
	}

	const std::string string(std::string s) {
		return s != "" ? '\"' + s + '\"' : "";
	}

	const std::string UInteger(const uint64_t num) {
		return std::to_string(num & 0x1fffffffffffff);   //just in case numbers are larger then 52 bits
	}

	const std::string optionalUInteger(const uint64_t num) {
		return num ? UInteger(num) : "";
	}

	const std::string integer(const int64_t num) {
		return std::to_string(num & 0x801FFFFFFFFFFFFF);  //just in case numbers are larger then 53 bits
	}

	const std::string optionalInteger(const int64_t num) {
		return num ? integer(num) : "";
	}

	const std::string boolean(const bool boolean) {
		return boolean ? "true" : "false";
	}
}