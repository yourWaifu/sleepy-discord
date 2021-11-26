#include "webhook.h"

namespace SleepyDiscord {
	Webhook::Webhook(const json::Value & json) :
		Webhook(json::fromJSON<Webhook>(json)) {
	}
}
