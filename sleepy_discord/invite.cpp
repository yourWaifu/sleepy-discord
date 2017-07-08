#include "invite.h"

SleepyDiscord::ChannelInvite::ChannelInvite() {
}

SleepyDiscord::ChannelInvite::~ChannelInvite() {
}

SleepyDiscord::ChannelInvite::ChannelInvite(const std::string * rawJson) {
	std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "name", "type" });
	id = values[0];
	name = values[1];
	type = values[2];
}

SleepyDiscord::ServerInvite::ServerInvite() {
}

SleepyDiscord::ServerInvite::~ServerInvite() {
}

SleepyDiscord::ServerInvite::ServerInvite(const std::string * rawJson) {
	std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "name", "splash", "icon" });
	id = values[0];
	name = values[1];
	splash = values[2];
	icon = values[3];
}

SleepyDiscord::Invite::Invite() {
}

SleepyDiscord::Invite::~Invite() {
}

SleepyDiscord::Invite::Invite(const std::string * rawJson) {
	std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "code", "guild", "channel" });
	code = values[0];
	server = ServerInvite(&values[1]);
	channel = ChannelInvite(&values[2]);
}
