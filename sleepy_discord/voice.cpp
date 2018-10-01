#include "voice.h"

namespace SleepyDiscord {
	VoiceState::VoiceState(const json::Value & json) :
		VoiceState(json::fromJSON<VoiceState>(json)) {
	}
	VoiceState::VoiceState(const nonstd::string_view & json) :
		VoiceState(json::fromJSON<VoiceState>(json)) {
	}

	VoiceRegion::VoiceRegion(const json::Value & json) :
		VoiceRegion(json::fromJSON<VoiceRegion>(json)) {
	}
	VoiceRegion::VoiceRegion(const nonstd::string_view & json) :
		VoiceRegion(json::fromJSON<VoiceRegion>(json)) {
	}

	VoiceServerUpdate::VoiceServerUpdate(const json::Value & json) :
		VoiceServerUpdate(json::fromJSON<VoiceServerUpdate>(json)) {
	}
	VoiceServerUpdate::VoiceServerUpdate(const nonstd::string_view & json) :
		VoiceServerUpdate(json::fromJSON<VoiceServerUpdate>(json)) {
	}
}
