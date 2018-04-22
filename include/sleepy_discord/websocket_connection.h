#pragma once
#include "websockets.h"

//custom dynamic
#ifdef SLEEPY_CUSTOM_WEBSOCKETS_CONNECTION
	#include "custom_websockets_connection.h"

	//defined
	#elif defined(SLEEPY_WEBSOCKETS_CONNECTION_INCLUDE) || defined(SLEEPY_WEBSOCKETS_CONNECTION)
		#ifdef SLEEPY_WEBSOCKETS_CONNECTION_INCLUDE
			#include SLEEPY_WEBSOCKETS_CONNECTION_INCLUDE
		#endif
		#ifdef SLEEPY_WEBSOCKETS_CONNECTION
			typedef SLEEPY_SESSION Session
		#endif

//defaults
#else
	#ifndef SLEEPY_LOCK_EXISTENT_TO
		#include "websocketpp_connection.h"
		#ifdef NONEXISTENT_WEBSOCKETPP
			#include "uwebsockets_connection.h"
			#ifdef NONEXISTENT_UWEBSOCKETS
				//last resort
				#include "custom_connection.h"
			#endif
		#endif
	#elif SLEEPY_LOCK_EXISTENT_TO == SLEEPY_WEBSOCKETPP
		#include "websocketpp_connection.h"
	#elif SLEEPY_LOCK_EXISTENT_TO == SLEEPY_UWEBSOCKETS
		#include "uwebsockets_connection.h"
	#else
		//last resort
		#include "custom_connection.h"
	#endif
#endif