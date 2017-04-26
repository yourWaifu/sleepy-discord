#pragma once
#include <cpr/cpr.h>
#ifndef NONEXISTENT_CPR
#include "http.h"

namespace SleepyDiscord {
	class CPRSession : public GenericSession {
	public:
		void setUrl(const std::string url);
		void setBody(const std::string jsonParameters);
		void setHeader(std::initializer_list<std::pair<std::string, std::string>> header);
		Response Post();
		Response Patch();
		Response Delete();
		Response Get();
		Response Put();
	private:
		Response request(const RequestMethod method);
		cpr::Session session;
	};

	typedef CPRSession Session;
}

#endif