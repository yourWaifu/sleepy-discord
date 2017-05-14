#include "webhook.h"

SleepyDiscord::Webhook::Webhook() {
}

SleepyDiscord::Webhook::Webhook(const std::string * rawJSON) {
	std::vector<std::string> values = json::getValues(rawJSON->c_str(),
	{ "id", "guild_id", "channel_id", "user", "name", "avatar", "token" });
	unsigned int i = -1;
	id		   = values[i++];
	server_id  = isDefined(values[i++]) ? values[i] : "";
	channel_id = values[i++];
	user	   = User(&values[i++]);
	name	   = isDefined(values[i++]) ? values[i] : "";
	avatar	   = isDefined(values[i++]) ? values[i] : "";
	token	   = values[i++];
}
