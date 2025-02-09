#pragma once

//custom dynamic
#ifdef SLEEPY_CUSTOM_WEBSOCKETS_CONNECTION
	#include "custom_connection.h"

//defined
#elif defined(SLEEPY_WEBSOCKETS_CONNECTION_INCLUDE) || defined(SLEEPY_WEBSOCKETS_CONNECTION)
	#ifdef SLEEPY_WEBSOCKETS_CONNECTION_INCLUDE
		#include SLEEPY_WEBSOCKETS_CONNECTION_INCLUDE
	#endif
	#ifdef SLEEPY_WEBSOCKETS_CONNECTION
		typedef SLEEPY_SESSION Session
	#endif

#elif defined(SLEEPY_DISCORD_CMAKE)
	#if defined(EXISTENT_ASIO)
		#include "asio_websocketconnection.h"
	#else
		#include "custom_connection.h"
	#endif

//defaults
#else
	#ifndef SLEEPY_LOCK_EXISTENT_TO
		#include "asio_websocketconnection.h"
		#ifdef NONEXISTENT_ASIO
			//last resort
			#include "custom_connection.h"
		#endif
	#elif SLEEPY_LOCK_EXISTENT_TO == SLEEPY_ASIO
		#include "asio_websocketconnection.h"
	#else
		//last resort
		#include "custom_connection.h"
	#endif
#endif