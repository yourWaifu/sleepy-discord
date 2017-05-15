#include "cpr_session.h"
#ifndef NONEXISTENT_CPR

namespace SleepyDiscord {
	void CPRSession::setHeader(const std::initializer_list<std::pair<std::string, std::string>>& header) {
		cpr::Header head;
		for (auto h : header) head.insert(h);
		session.SetHeader(head);
	}

	void CPRSession::setMultipart(const std::initializer_list<Part>& parts) {
		std::vector<cpr::Part> cprParts;
		for (auto m : parts) {
			if (m.isFile) cprParts.push_back(cpr::Part(m.name, cpr::File(m.value)));
			else          cprParts.push_back(cpr::Part(m.name, m.value));
		}
		cpr::Multipart muiltpart({});
		muiltpart.parts = cprParts;
		session.SetMultipart(muiltpart);
	}

	Response CPRSession::convertResponse(cpr::Response response) {
		Response target;
		target.statusCode = response.status_code;
		target.text = response.text;
		for (auto i : response.header) {
			target.header.insert(i);
		}
		return target;
	}
}
#endif