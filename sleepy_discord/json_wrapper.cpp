#include "json.h"

namespace json {

	std::vector<std::string> getValues(const char* source, std::initializer_list<const char *const> names) {
		if (*source == 0) return std::vector<std::string>{0};
		const unsigned int numOfValues = names.size();
		std::vector<JSON_findMuitipleStruct>values(numOfValues);
		unsigned int ii = -1;
		for (const char *const name : names)
			values[++ii] = { name, 0, strlen(name), 0 };
		JSON_findMuitiple(numOfValues, source, &values[0]);
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
		JSON_findMuitiple(1, source, &value);
		return std::string(source + value.namePosition, value.valueLength);
	}

	void getArray(const std::string* _source, std::vector<std::string>* target) {
		getArray<std::string>(_source, target, [](std::string* value, std::string string) {*value = string;});
	}

}