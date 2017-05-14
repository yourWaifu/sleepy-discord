#pragma once
#include "json.h"

namespace SleepyDiscord {

	class DiscordObject {
	public:
		DiscordObject() {}

	protected:
		inline bool getBool(const std::string& string) {
			return string[0] == 't';
		}

		inline bool isDefined(const std::string& string) {
			return string[0] && string[0] != 'n';
		}

		inline bool isDefault(const std::string& string) {
			return string == "" || string[0] == 'n';
		}
	};

	template <class _DiscordObject>
	std::vector<_DiscordObject> JSON_getArray(const std::string* _source) {
		return json::getArray<_DiscordObject>(_source, [](_DiscordObject* value, std::string string) {*value = _DiscordObject(&string); });
	}
}