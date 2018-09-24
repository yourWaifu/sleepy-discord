#include "invite.h"

namespace SleepyDiscord {
	Invite::Invite(const json::Value & json) :
		Invite(json::fromJSON<Invite>(json)) {
	}
	Invite::Invite(const nonstd::string_view & json) :
		Invite(json::fromJSON<Invite>(json)) {
	}

	InviteMetadata::InviteMetadata(const json::Value & json) :
		InviteMetadata(json::fromJSON<InviteMetadata>(json)) {
	}
	InviteMetadata::InviteMetadata(const nonstd::string_view & json) :
		InviteMetadata(json::fromJSON<InviteMetadata>(json)) {
	}
}
