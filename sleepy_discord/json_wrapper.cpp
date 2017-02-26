#include "json.h"

namespace json {

	void getValues(const char* source, const char *const*const names, std::string * targets, const unsigned int numOfValues) {
		if (*source == 0) return;
		std::vector<JSON_findMuitipleStruct>values(numOfValues);
		for (unsigned int i = 0; i < numOfValues; i++) {
			values[i] = { names[i], 0, strlen(names[i]), 0 };
		}
		JSON_findMuitiple(numOfValues, source, &values[0]);
		for (unsigned int i = 0; i < numOfValues; i++) {
			if (0 < values[i].namePosition) {
				targets[i].assign(source + values[i].namePosition, values[i].valueLength);	//I think this is why we need a c string for the source
			}
		}
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