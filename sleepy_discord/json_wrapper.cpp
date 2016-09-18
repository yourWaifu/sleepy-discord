#include "json.h"

template<>
std::string JSON_find<std::string>(const char * name, const char * source) {
	const unsigned int startPosition = JSON_find(name, source);
	if (startPosition == NULL) return "";
	switch (source[startPosition]) {
	case '"': {
		const unsigned int stringLength = JSON_measureString(source, &startPosition);
		std::string result/* = source + startPosition + 1*/;    //the + 1 will remove the " at the beginning
		result.assign(source + startPosition + 1, stringLength);                    //we are - 1 because we removed the "
		return result;
	} break;
	default:
		break;
	}
	return "";
}