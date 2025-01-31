#pragma once

#define SLEEPY_DEFINE_CUSTOM_CLIENT \
namespace SleepyDiscord {\
typedef BaseDiscordClient DiscordClient;\
}

#ifdef SLEEPY_CUSTOM_CLIENT
	#include "client.h"
	SLEEPY_DEFINE_CUSTOM_CLIENT
#elif defined(SLEEPY_DISCORD_CMAKE)
	#if defined(EXISTENT_ASIO)
		#include "asio_websocket.h"
	#else
		#include "client.h"
		SLEEPY_DEFINE_CUSTOM_CLIENT
	#endif
#else
	#include "asio_websocket.h"
	#ifdef NONEXISTENT_ASIO
		#include "client.h"
		SLEEPY_DEFINE_CUSTOM_CLIENT
	#endif
#endif

namespace SleepyDiscord {
	
}