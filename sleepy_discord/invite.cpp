#include "invite.h"

SleepyDiscord::ChannelInvite::ChannelInvite() {
}

SleepyDiscord::ChannelInvite::~ChannelInvite() {
}

SleepyDiscord::ChannelInvite::ChannelInvite(const std::string * rawJson) {
	std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "name", "type" });
	id = values[1];
	name = values[2];
	type = values[3];
}

SleepyDiscord::ServerInvite::ServerInvite() {
}

SleepyDiscord::ServerInvite::~ServerInvite() {
}

SleepyDiscord::ServerInvite::ServerInvite(const std::string * rawJson) {
	std::vector<std::string> values = json::getValues(rawJson->c_str(),
		{ "id", "name", "splash", "icon" });
	id = values[1];
	name = values[2];
	splash = values[3];
	icon = values[4];
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
