#include "stage_instance.h"

namespace SleepyDiscord {
	StageInstance::StageInstance(const json::Value& json) :
		StageInstance(json::fromJSON<StageInstance>(json)) {
	}
}