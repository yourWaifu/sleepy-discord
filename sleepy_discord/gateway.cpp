#include "gateway.h"

namespace SleepyDiscord {
	SessionStartLimit::SessionStartLimit(const json::Value& json) :
		SessionStartLimit(json::fromJSON<SessionStartLimit>(json)) {
	}
	Gateway::Gateway(const json::Value& json) :
		Gateway(json::fromJSON<Gateway>(json)) {
	}
	Ready::Ready(const json::Value & json) :
		Ready(json::fromJSON<Ready>(json)) {
	}
	ActivityTimestamp::ActivityTimestamp(const json::Value & json) :
		ActivityTimestamp(json::fromJSON<ActivityTimestamp>(json)) {
	}
	ActivityParty::ActivityParty(const json::Value & json) :
		ActivityParty(json::fromJSON<ActivityParty>(json)) {
	}
	ActivityAssets::ActivityAssets(const json::Value & json) :
		ActivityAssets(json::fromJSON<ActivityAssets>(json)) {
	}
	ActivitySecrets::ActivitySecrets(const json::Value & json) :
		ActivitySecrets(json::fromJSON<ActivitySecrets>(json)) {
	}
	Activity::Activity(const json::Value & json) :
		Activity(json::fromJSON<Activity>(json)) {
	}
	PresenceUpdate::PresenceUpdate(const json::Value & json) :
		PresenceUpdate(json::fromJSON<PresenceUpdate>(json)) {
	}
	ServerMembersChunk::ServerMembersChunk(const json::Value & json) :
		ServerMembersChunk(json::fromJSON<ServerMembersChunk>(json)) {
	}
}