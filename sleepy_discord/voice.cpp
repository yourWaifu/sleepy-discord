#include "voice.h"

namespace SleepyDiscord {
	VoiceState::VoiceState() : deaf(false), mute(false), selfDeaf(false), selfMute(false),
		suppress(false) {}
	VoiceState::VoiceState(const std::string * rawJSON) : VoiceState(json::getValues(rawJSON->c_str(), fields)) {}

	VoiceState::VoiceState(const std::vector<std::string> values) :
		//variable      modifier value               felid
		serverID (        values[index(fields, "guild_id"  )] ),
		channelID(        values[index(fields, "channel_id")] ),
		userID   (        values[index(fields, "user_id"   )] ),
		sessionID(        values[index(fields, "session_id")] ),
		deaf     (getBool(values[index(fields, "deaf"      )])),
		mute     (getBool(values[index(fields, "mute"      )])),
		selfDeaf (getBool(values[index(fields, "self_deaf" )])),
		selfMute (getBool(values[index(fields, "self_mute" )])),
		suppress (getBool(values[index(fields, "suppress"  )]))
	{}

	const std::initializer_list<const char*const> VoiceState::fields = {
		"guild_id", "channel_id", "user_id", "session_id", "deaf", "mute",
		"self_deaf", "self_mute", "suppress"
	};
	
	VoiceRegion::VoiceRegion() : samplePort(0), vip(false), optimal(false), deprecated(false), custom(false) {}

	VoiceRegion::VoiceRegion(const std::string * rawJSON) : VoiceRegion(json::getValues(rawJSON->c_str(), fields)) {}

	VoiceRegion::VoiceRegion(const std::vector<std::string> values) :
		//variable      modifier value               felid
		ID             (        values[index(fields, "id"             )] ),
		name           (        values[index(fields, "name"           )] ),
		sampleHostname (        values[index(fields, "sample_hostname")] ),
		samplePort     (toInt  (values[index(fields, "sample_port"    )])),
		vip            (getBool(values[index(fields, "vip"            )])),
		optimal        (getBool(values[index(fields, "optimal"        )])),
		deprecated     (getBool(values[index(fields, "deprecated"     )])),
		custom         (getBool(values[index(fields, "custom"         )]))
	{}

	const std::initializer_list<const char*const> VoiceRegion::fields = { 
		"id", "name", "sample_hostname", "sample_port" , "vip", "optimal",
		"deprecated", "custom"
	};

	const std::initializer_list<const char*const> VoiceServerUpdate::fields = {
		"token", "guild_id", "endpoint"
	};

	VoiceServerUpdate::VoiceServerUpdate() {}

	VoiceServerUpdate::VoiceServerUpdate(const std::string * rawJSON) : VoiceServerUpdate(json::getValues(rawJSON->c_str(), fields)) {}

	VoiceServerUpdate::VoiceServerUpdate(const std::vector<std::string> values) :
		//variable value              felid
		token   (values[index(fields, "token"   )]),
		serverID(values[index(fields, "guild_id")]),
		endpoint(values[index(fields, "endpoint")])
	{}
}
