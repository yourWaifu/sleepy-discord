#pragma once
#include <memory>
#include "http.h"

namespace SleepyDiscord {
	typedef GenericSession* (*const CustomInit)();  //keep compatibility with old code
	typedef CustomInit CustomInitSession;

	class CustomSession : public GenericSession {
	public:
		static CustomInitSession init;
		CustomSession() : session(init()) {}
		inline void setUrl(const std::string& url) { session->setUrl(url); }
		inline void setBody(const std::string* jsonParamters) { session->setBody(jsonParamters); }
		inline void setHeader(const std::vector<HeaderPair>& header) {
			session->setHeader(header);
		}
		inline void setMultipart(const std::initializer_list<Part>& parts) {
			session->setMultipart(parts);
		}
		inline Response Post  () { return session->Post  (); }
		inline Response Patch () { return session->Patch (); }
		inline Response Delete() { return session->Delete(); }
		inline Response Get   () { return session->Get   (); }
		inline Response Put   () { return session->Put   (); }
	private:
		std::unique_ptr<GenericSession> session; //pointer to session
	};

	typedef CustomSession Session;
}