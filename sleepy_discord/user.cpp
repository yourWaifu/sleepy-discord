#include "user.h"

namespace SleepyDiscord {
	User::User(const json::Value & json) :
		User(json::fromJSON<User>(json)) {
	}
	User::User(const nonstd::string_view & json) :
		User(json::fromJSON<User>(json)) {
	}

	Connection::Connection(const json::Value & json) :
		Connection(json::fromJSON<Connection>(json)) {
	}
	Connection::Connection(const nonstd::string_view & json) :
		Connection(json::fromJSON<Connection>(json)) {
	}
}