#pragma once
#include <cpr/cpr.h>
#ifndef NONEXISTENT_CPR
#include "http.h"

namespace SleepyDiscord {
	class CPRSession : public GenericSession {
	public:
		inline void setUrl(const std::string& url) {
			session.SetUrl(cpr::Url{ url });
		}
		inline void setBody(const std::string* jsonParameters) {
			session.SetBody(cpr::Body{ *jsonParameters });
		}
		void setHeader(const std::initializer_list<std::pair<std::string, std::string>>& header);
		void setMultipart(const std::initializer_list<Part>& parts);
		inline Response Post  () { return convertResponse(session.Post  ()); }
		inline Response Patch () { return convertResponse(session.Patch ()); }
		inline Response Delete() { return convertResponse(session.Delete()); }
		inline Response Get   () { return convertResponse(session.Get   ()); }
		inline Response Put   () { return convertResponse(session.Put   ()); }
	private:
		Response convertResponse(cpr::Response response);
		cpr::Session session;
	};

	typedef CPRSession Session;
}

#endif