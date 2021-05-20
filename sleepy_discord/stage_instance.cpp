#include "stage_instance.h"

namespace SleepyDiscord {
	StageInstance::StageInstance(const json::Value& json) :
		StageInstance(json::fromJSON<StageInstance>(json)) {
	}
	StageInstance::StageInstance(const nonstd::string_view& json) :
		StageInstance(json::fromJSON<StageInstance>(json)) {
	}
}