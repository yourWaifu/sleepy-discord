#include "json.h"

void JSON_getValues(const char* source, const char ** names, std::string * targets, const unsigned int numOfValues) {
	std::vector<JSON_findMuitipleStruct>values(numOfValues);
	//std::vector<unsigned int>namePositions(numOfValues);
	//std::vector<unsigned int>sizes(numOfValues);
	for (unsigned int i = 0; i < numOfValues; i++) {
		values[i] = { names[i], 0, strlen(names[i]), 0 };
		//sizes[i] = strlen(names[i]);
	}
	//JSON_findMuitiple(names, numOfValues, source, &namePositions[0], &sizes[0]);
	JSON_findMuitiple(numOfValues, source, &values[0]);
	for (unsigned int i = 0; i < numOfValues; i++) {
		if (0 < values[i].namePosition) {
			targets[i].assign(source + values[i].namePosition, values[i].valueLength);	//I think this is why we need a c string for the source
		}
	}
}

void JSON_getValue(const char* source, const char * name, std::string* target) {
	JSON_findMuitipleStruct value = { name, 0, strlen(name), 0 };
	JSON_findMuitiple(1, source, &value);
	target->assign(source + value.namePosition, value.valueLength);
}