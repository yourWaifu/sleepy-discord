#include "webhook.h"

namespace SleepyDiscord {
	Webhook::Webhook(const json::Value & json) :
		Webhook(json::fromJSON<Webhook>(json)) {
	}
	Webhook::Webhook(const nonstd::string_view & json) :
		Webhook(json::fromJSON<Webhook>(json)) {
	}
}
