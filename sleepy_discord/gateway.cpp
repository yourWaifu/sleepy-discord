#include "gateway.h"

namespace SleepyDiscord {
	Ready::Ready(const json::Value & json) :
		Ready(json::fromJSON<Ready>(json)) {
	}
	Ready::Ready(const nonstd::string_view & json) :
		Ready(json::fromJSON<Ready>(json)) {
	}
	ActivityTimestamp::ActivityTimestamp(const json::Value & json) :
		ActivityTimestamp(json::fromJSON<ActivityTimestamp>(json)) {
	}
	ActivityTimestamp::ActivityTimestamp(const nonstd::string_view & json) :
		ActivityTimestamp(json::fromJSON<ActivityTimestamp>(json)) {
	}
	ActivityParty::ActivityParty(const json::Value & json) :
		ActivityParty(json::fromJSON<ActivityParty>(json)) {
	}
	ActivityParty::ActivityParty(const nonstd::string_view & json) :
		ActivityParty(json::fromJSON<ActivityParty>(json)) {
	}
	ActivityAssets::ActivityAssets(const json::Value & json) :
		ActivityAssets(json::fromJSON<ActivityAssets>(json)) {
	}
	ActivityAssets::ActivityAssets(const nonstd::string_view & json) :
		ActivityAssets(json::fromJSON<ActivityAssets>(json)) {
	}
	ActivitySecrets::ActivitySecrets(const json::Value & json) :
		ActivitySecrets(json::fromJSON<ActivitySecrets>(json)) {
	}
	ActivitySecrets::ActivitySecrets(const nonstd::string_view & json) :
		ActivitySecrets(json::fromJSON<ActivitySecrets>(json)) {
	}
	Activity::Activity(const json::Value & json) :
		Activity(json::fromJSON<Activity>(json)) {
	}
	Activity::Activity(const nonstd::string_view & json) :
		Activity(json::fromJSON<Activity>(json)) {
	}
	PresenceUpdate::PresenceUpdate(const json::Value & json) :
		PresenceUpdate(json::fromJSON<PresenceUpdate>(json)) {
	}
	PresenceUpdate::PresenceUpdate(const nonstd::string_view & json) :
		PresenceUpdate(json::fromJSON<PresenceUpdate>(json)) {
	}
}