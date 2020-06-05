#include "channel.h"

namespace SleepyDiscord {
	Channel::~Channel() {

	}

	Channel::Channel(const json::Value& json) :
		Channel(json::fromJSON<Channel>(json)) {
	}

	Channel::Channel(const nonstd::string_view& json) :
		Channel(json::fromJSON<Channel>(json)) {
	}

	Overwrite::Overwrite(const json::Value& json) :
		Overwrite(json::fromJSON<Overwrite>(json)) {
	}

	Overwrite::Overwrite(const nonstd::string_view& json) :
		Overwrite(json::fromJSON<Overwrite>(json)) {
	}
}
