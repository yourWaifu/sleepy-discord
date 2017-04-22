#include "cpr_session.h"
#ifndef NONEXISTENT_CPR

namespace SleepyDiscord {
	void CPRSession::setUrl(const std::string url) {
		session.SetUrl(cpr::Url{ url });
	}

	void CPRSession::setBody(const std::string jsonParameters) {
		session.SetBody(cpr::Body{ jsonParameters });
	}

	void CPRSession::setHeader(std::initializer_list<std::pair<std::string, std::string>> header) {
		cpr::Header head;
		for (auto h : header) head.insert(h);
		session.SetHeader(head);
	}

	Response CPRSession::Post() {
		return request(RequestMethod::Post);
	}

	Response CPRSession::Patch() {
		return request(RequestMethod::Patch);
	}

	Response CPRSession::Delete() {
		return request(RequestMethod::Delete);
	}

	Response CPRSession::Get() {
		return request(RequestMethod::Get);
	}

	Response CPRSession::Put() {
		return request(RequestMethod::Put);
	}

	Response CPRSession::request(const RequestMethod method) {
		cpr::Response response;
		switch (method) {
		case RequestMethod::Post:   response = session.Post(); break;
		case RequestMethod::Patch:  response = session.Patch(); break;
		case RequestMethod::Delete: response = session.Delete(); break;
		case RequestMethod::Get:    response = session.Get(); break;
		case RequestMethod::Put:    response = session.Put(); break;
		}
		Response r;
		r.statusCode = response.status_code;
		r.text = response.text;
		for (auto i : response.header) {
			r.header.insert(i);
		}
		return r;
	}
}
#endif