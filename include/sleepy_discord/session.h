#pragma once
#include "http.h"

//custom dynamic
#ifdef SLEEPY_CUSTOM_SESSION
	#include "custom_session.h"

	//defined
	#elif defined(SLEEPY_SESSION) || defined(SLEEPY_SESSION_INCLUDE)
		#ifdef SLEEPY_SESSION_INCLUDE
			#include SLEEPY_SESSION_INCLUDE
		#endif
		#ifdef SLEEPY_SESSION
			typedef SLEEPY_SESSION Session
		#endif

	//defaults
#elif defined(SLEEPY_DISCORD_CMAKE)
	#if defined(EXISTENT_CPR)
		#include "cpr_session.h"
	#else
		#include "custom_session.h"
	#endif
#else
	#include "cpr_session.h"
	#ifdef NONEXISTENT_CPR

		//last resort
		#include "custom_session.h"
	#endif
#endif