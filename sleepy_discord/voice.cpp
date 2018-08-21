#include "voice.h"

namespace SleepyDiscord {
	VoiceRegion::VoiceRegion(const std::string * rawJSON) : VoiceRegion(json::getValues(rawJSON->c_str(), fields)) {}

	VoiceRegion::VoiceRegion(const std::vector<std::string> values) :
		//variable      modifier value               felid
		Parent         (        values[index(fields, "id"             )] ),
		name           (        values[index(fields, "name"           )] ),
		sampleHostname(        values[index(fields, "sample_hostname")] ),
		samplePort    (toInt  (values[index(fields, "sample_port"    )])),
		vip            (getBool(values[index(fields, "vip"            )])),
		optimal        (getBool(values[index(fields, "optimal"        )])),
		deprecated     (getBool(values[index(fields, "deprecated"     )])),
		custom         (getBool(values[index(fields, "custom"         )]))
	{}

	const std::initializer_list<const char*const> VoiceRegion::fields = { 
		"id", "name", "sample_hostname", "sample_port" , "vip", "optimal",
		"deprecated", "custom"
	};
}
