#pragma once
#include "json.h"

namespace SleepyDiscord {
	class DiscordObject {
	public:
		void fillOut(JSON* _JSON);
		virtual void fillOut(JSON_object * _JSON_object);
		virtual void fillOut(const char* name, void * value) = 0;
	};

	template <class _DiscordObject> void defaultFillfunction(_DiscordObject* arrayValue, JSON_array* _array, unsigned int index) {
		arrayValue->_DiscordObject::fillOut((JSON_object*)JSON_accessArray(_array, index));
	}

	/*
	this will convert a parsed json array into an array of whatever type you want the data from the json to be
	this is needed because the json parser give you an array in the JSON_array structure (check the json.h file for the JSON_array structure)
	this makes and returns a new array that's not a void without the type enumeration
	the fillFunction parameter is a function that is called for each value in the json array
		the first parameter of fillFunction is a pointer to the value in the array that will be returned
		the 2nd parameter is the pointer to the parsed json array
		the last parameter is the index to the value in the parsed json array
	*/
	template <class _DiscordObject> _DiscordObject* fillOutArray(void* value, unsigned int& numberOfobjects, void (*fillFunction) (_DiscordObject*, JSON_array*, unsigned int) = defaultFillfunction) {
		JSON_array* _array = (JSON_array*)value;
		_DiscordObject* object = new _DiscordObject[_array->count];
		for (unsigned int i = 0; i < _array->count; i++) {
			fillFunction(object + i, _array, i);
		}
		numberOfobjects = _array->count;
		return object;
	};
}