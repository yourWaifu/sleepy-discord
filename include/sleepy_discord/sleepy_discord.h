#pragma once

#define SLEEPY_DEFINE_CUSTOM_CLIENT \
namespace SleepyDiscord {\
typedef BaseDiscordClient DiscordClient;\
}

#ifdef SLEEPY_CUSTOM_CLIENT
	#include "client.h"
	SLEEPY_DEFINE_CUSTOM_CLIENT
#elif defined(SLEEPY_DISCORD_CMAKE)
	#if defined(EXISTENT_WEBSOCKETPP)
		#include "websocketpp_websocket.h"
	#elif defined(EXISTENT_UWEBSOCKETS)
		#include "uwebsockets_websocket.h"
	#elif defined(EXISTENT_ASIO)
		#include "asio_websocket.h"
	#else
		#include "client.h"
		SLEEPY_DEFINE_CUSTOM_CLIENT
	#endif
#else
	#include "websocketpp_websocket.h"
	#ifdef NONEXISTENT_WEBSOCKETPP
		#include "uwebsockets_websocket.h"
		#ifdef NONEXISTENT_UWEBSOCKETS
			#include "asio_websocket.h"
			#ifdef NONEXISTENT_ASIO
				#include "client.h"
				SLEEPY_DEFINE_CUSTOM_CLIENT
			#endif
		#endif
	#endif
#endif

namespace SleepyDiscord {
	
}