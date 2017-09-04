#include "json.h"
#include <stdexcept>

namespace SleepyDiscord { namespace json {
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
		const char* source = _source->c_str();
		if (source[0] != '[' && source[1] == ']') return std::vector<std::string>();

		//get size of array and change size of target array
		const unsigned int sourceLength = _source->size();
		unsigned int arraySize = 0;
		unsigned int position = 1;
		for (; position < sourceLength; position++) {
			switch (source[position]) {
			case '"': JSON_skipString(source, &position); break;
			case '{': JSON_skipObject(source, &position); break;
			case '[': JSON_skipObject(source, &position); break;
			case ',': ++arraySize; break;
			default: break;
			}
		}
		for (bool loop = true; loop;) {	//count last variable
			switch (source[--position]) {
			case '[': return std::vector<std::string>(); break;	//empty array
			case ' ': break;
			case ']': break;
			default:
				loop = false;
				++arraySize;
				break;
			}
		}
		std::vector<std::string> target(arraySize);

		//fill the vector with variables
		position = 1;
		for (unsigned int index = 0; index < arraySize && position < sourceLength; position++) {	//variables should be the same type, right?
			switch (source[position]) {
			case '"': {
				const unsigned int size = JSON_measureString(source, &position) - 1;	//the -1 removes the "
				target[index++] = std::string(source + position + 1, size);
				position += size + 1;	//the +1 should skip the "
			} break;	//This should make a string, and the +1 and -1 removes the two "
			case '{': {
				const unsigned int size = JSON_measureObject(source, &position) + 1;	//the +1 adds a }
				target[index++] = std::string(source + position, size);
				position += size;
			} break;
			}
		}
		return target;
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