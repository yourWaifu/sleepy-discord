#include "voice.h"

SleepyDiscord::VoiceRegion::VoiceRegion(const std::string * rawJson) {
	std::vector<std::string> values = json::getValues(rawJson->c_str(),
	{ "id", "name", "sample_hostname", "sample_port" , "vip", "optimal",
		"deprecated", "custom" });

	unsigned int i = -1;
	id              = values[++i];
	name            = values[++i];
	sample_hostname = values[++i];
	sample_port     = std::stoi(values[++i]);
	vip             = getBool(values[++i]);
	optimal         = getBool(values[++i]);
	deprecated      = getBool(values[++i]);
	custom          = getBool(values[++i]);
}
