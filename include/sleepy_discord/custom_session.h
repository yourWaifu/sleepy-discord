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
		inline void setMultipart(const std::vector<Part>& parts) {
			session->setMultipart(parts);
		}
		inline void setResponseCallback(const ResponseCallback& callback) {
			session->setResponseCallback(callback);
		}
		inline Response request(RequestMethod method) {
			return session->request(method);
		}
	private:
		std::unique_ptr<GenericSession> session; //pointer to session
	};

	typedef CustomSession Session;
}
