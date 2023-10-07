#include "thread.h"

namespace SleepyDiscord {
	ThreadMember::ThreadMember(const json::Value& json) :
		ThreadMember(json::fromJSON<ThreadMember>(json)) {
	}

	ThreadListSync::ThreadListSync(const json::Value& json) :
		ThreadListSync(json::fromJSON<ThreadListSync>(json)) {
	}

	ThreadMembersUpdate::ThreadMembersUpdate(const json::Value& json) :
		ThreadMembersUpdate(json::fromJSON<ThreadMembersUpdate>(json)) {
	}
}