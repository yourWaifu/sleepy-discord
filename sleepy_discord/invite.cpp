#include "invite.h"

namespace SleepyDiscord {
	Invite::Invite(const json::Value & json) :
		Invite(json::fromJSON<Invite>(json)) {
	}

	InviteMetadata::InviteMetadata(const json::Value & json) :
		InviteMetadata(json::fromJSON<InviteMetadata>(json)) {
	}
}
