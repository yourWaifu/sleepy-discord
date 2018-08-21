#include "invite.h"

namespace SleepyDiscord {
	ChannelInvite::ChannelInvite() {
	}

	ChannelInvite::~ChannelInvite() {
	}

	ChannelInvite::ChannelInvite(const std::string * rawJSON) : ChannelInvite(json::getValues(rawJSON->c_str(), fields)) {}

	ChannelInvite::ChannelInvite(const std::vector<std::string> values) :
		//variable value               felid
		Parent(values[index(fields, "id"  )]),
		name  (values[index(fields, "name")]),
		type  (values[index(fields, "type")])
	{}

	const std::initializer_list<const char*const> ChannelInvite::fields = {
		"id", "name", "type"
	};

	ServerInvite::ServerInvite() {
	}

	ServerInvite::~ServerInvite() {
	}

	ServerInvite::ServerInvite(const std::string * rawJSON) : ServerInvite(json::getValues(rawJSON->c_str(), fields)) {}

	ServerInvite::ServerInvite(const std::vector<std::string> values) :
		//variable value                 felid
		Parent(values[index(fields, "id"    )]),
		name  (values[index(fields, "name"  )]),
		splash(values[index(fields, "splash")]),
		icon  (values[index(fields, "icon"  )])
	{}

	const std::initializer_list<const char*const> ServerInvite::fields = {
		"id", "name", "splash", "icon"
	};

	Invite::Invite() {
	}

	Invite::~Invite() {
	}

	Invite::Invite(const std::string * rawJSON) : Invite(json::getValues(rawJSON->c_str(), fields)) {}

	Invite::Invite(const std::vector<std::string> values) :
		//variable modifier    value                     felid
		code   (               values[index(fields, "code"   )] ),
		server (ServerInvite( &values[index(fields, "guild"  )])),
		channel(ChannelInvite(&values[index(fields, "channel")]))
	{}

	const std::initializer_list<const char*const> Invite::fields = {
		"code", "guild", "channel"
	};
}
