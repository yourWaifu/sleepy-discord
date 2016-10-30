#include "json.h"

template<>
std::string JSON_find<std::string>(const char * name, const char * source) {
	const unsigned int startPosition = JSON_find(name, source);
	if (startPosition == NULL) return "";
	switch (source[startPosition]) {
	case '"': {
		const unsigned int stringLength = JSON_measureString(source, &startPosition);
		std::string result/*  = source + startPosition + 1*/;    //the + 1 will remove the " at the beginning
		result.assign(source + startPosition + 1, stringLength);                    //we are - 1 because we removed the "
		return result;
	} break;
	default:
		break;
	}
	return "";
}

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