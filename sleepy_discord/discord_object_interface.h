#pragma once
#include "json.h"

namespace SleepyDiscord {
	class DiscordObject {
	public:
		void fillOut(JSON* _JSON);
		virtual void fillOut(JSON_object * _JSON_object);
		virtual void fillOut(const char* name, void * value) = 0;
	};

	template <class _DiscordObject> _DiscordObject* fillOutArray(void* value, unsigned int& numberOfobjects) {
		JSON_array* _array = (JSON_array*)value;
		_DiscordObject* object = new _DiscordObject[_array->count];
		for (unsigned int i = 0; i < _array->count; i++) {
			object[i]._DiscordObject::fillOut((JSON_object*)JSON_accessArray(_array, i));
		}
		numberOfobjects = _array->count;
		return object;
	};
}