#include "cpr_session.h"
#ifndef NONEXISTENT_CPR

namespace SleepyDiscord {
	void CPRSession::setHeader(const std::vector<HeaderPair>& header) {
		cpr::Header head;
		for (HeaderPair pair : header)
			head.insert({ pair.name, pair.value });
		session.SetHeader(head);
	}

	void CPRSession::setMultipart(const std::vector<Part>& parts) {
		std::vector<cpr::Part> cprParts;
		for (Part const & m : parts) {
			if (m.isFile) cprParts.push_back(cpr::Part(m.name, cpr::File(m.value)));
			else          cprParts.push_back(cpr::Part(m.name, m.value));
		}

		muiltpart.parts = cprParts;
		session.SetMultipart(muiltpart);
	}

	Response CPRSession::request(RequestMethod method) {
		return perform(method);
	}

	Response CPRSession::perform(RequestMethod method) {
		cpr::Response response;
		switch (method) {
		case Post  : response = session.Post  (); break;
		case Patch : response = session.Patch (); break;
		case Delete: response = session.Delete(); break;
		case Get   : response = session.Get   (); break;
		case Put   : response = session.Put   (); break;
		default    : return Response(); break;
		}

		Response target;
		target.statusCode = response.status_code;
		target.text = response.text;
		for (std::pair<std::string, std::string> i : response.header) {
			target.header.insert(i);
		}
		return target;
	}
}
#endif
