#include "attachment.h"

namespace SleepyDiscord {
	Attachment::Attachment(const json::Value& json) :
		Attachment(json::fromJSON<Attachment>(json)) {
	}
}