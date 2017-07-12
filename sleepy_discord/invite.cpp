#include "invite.h"

SleepyDiscord::ChannelInvite::ChannelInvite() {
}

SleepyDiscord::ChannelInvite::~ChannelInvite() {
}

SleepyDiscord::ChannelInvite::ChannelInvite(const std::string * rawJson) {
	std::initializer_list<const char*const> names = {
		"id", "name", "type"
	};

	std::vector<std::string> values = json::getValues(rawJson->c_str(), names);
	id   = values[index(names, "id"  )];
	name = values[index(names, "name")];
	type = values[index(names, "type")];
}

SleepyDiscord::ServerInvite::ServerInvite() {
}

SleepyDiscord::ServerInvite::~ServerInvite() {
}

SleepyDiscord::ServerInvite::ServerInvite(const std::string * rawJson) {
	std::initializer_list<const char*const> names = {
		"id", "name", "splash", "icon"
	};

	std::vector<std::string> values = json::getValues(rawJson->c_str(), names);
	id     = values[index(names, "id"    )];
	name   = values[index(names, "name"  )];
	splash = values[index(names, "splash")];
	icon   = values[index(names, "icon"  )];
}

SleepyDiscord::Invite::Invite() {
}

SleepyDiscord::Invite::~Invite() {
}

SleepyDiscord::Invite::Invite(const std::string * rawJson) {
	std::initializer_list<const char*const> names = {
		"code", "guild", "channel"
	};

	std::vector<std::string> values = json::getValues(rawJson->c_str(), names);
	code    =                values[index(names, "code"   )] ;
	server  = ServerInvite( &values[index(names, "guild"  )]);
	channel = ChannelInvite(&values[index(names, "channel")]);
}
