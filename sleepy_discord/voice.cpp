#include "voice.h"

namespace SleepyDiscord {
	VoiceState::VoiceState(const json::Value & json) :
		VoiceState(json::fromJSON<VoiceState>(json)) {
	}

	VoiceRegion::VoiceRegion(const json::Value & json) :
		VoiceRegion(json::fromJSON<VoiceRegion>(json)) {
	}

	VoiceServerUpdate::VoiceServerUpdate(const json::Value & json) :
		VoiceServerUpdate(json::fromJSON<VoiceServerUpdate>(json)) {
	}
}
