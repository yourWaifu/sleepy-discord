#pragma once
#include "http.h"

namespace SleepyDiscord {
	class CustomSession : public GenericSession {
	public:
		
	private:
		void * session; //pointer to the customSession
	};

	typedef CustomSession Session;
}