#pragma once
#include <stdlib.h>
#include "sleepy_discord/session.h"
#include <emscripten.h>
#include <iostream> //TODO remove after debugging

extern "C" {
	extern int performRequest(const char* methodName, const char* url, const char** header, int headerSize, int sizeofPointer, const char* body);
	extern int getResponseStatusCode(int handle);
	extern char* getResponseBody(int handle);
	extern char* getResponseHeader(int handle);
	extern void destroySession(int handle);
}

class WebSession : public SleepyDiscord::GenericSession {
private:
	const std::string* url = nullptr;
	const std::string* body = nullptr;
	std::vector<const char*> header;

	char* responseBody = nullptr;

	SleepyDiscord::Response request(SleepyDiscord::RequestMethod method) {
		int handle = performRequest(getMethodName(method), url->data(), &header[0], header.size(), sizeof(const char *), body->data());
		for (const char* p : header) {
			std::cout << p << '\n';
		}

		int responseCode = getResponseStatusCode(handle);

		char* rawResponseHeader = getResponseHeader(handle);
		std::string rawHeader = rawResponseHeader;
		std::map<std::string, std::string> responseHeader;
		//basically copied straight from my 3DS port with a different condition for the for loop
		//TODO: I should make function for this to stop code reuse
		const std::string newLine = "\r\n";
		size_t offset = 0;
		for (size_t newLinePos = rawHeader.find(newLine, offset);    //variable for the end of header
			offset < rawHeader.length();                //condition
			newLinePos = rawHeader.find(newLine, offset)) {
			const std::string headerFieldLeft = rawHeader.substr(offset, rawHeader.find(':', offset) - offset);
			offset += headerFieldLeft.length();
			while (rawHeader[++offset] == ' ');	//ignore whitespace
			const std::string headerFieldRight = rawHeader.substr(offset, newLinePos - offset);
			responseHeader[headerFieldLeft] = headerFieldRight;
			offset = newLinePos + newLine.length();
		}

		responseBody = getResponseBody(handle);

		//clean up
		destroySession(handle);
		free(rawResponseHeader);

		return { responseCode, responseBody, responseHeader };
	}

public:
	WebSession() {}
	~WebSession() {
		//Strings converted from JS were allocated and need to be freed
		if (responseBody != nullptr) free(responseBody);
	}
	inline void setUrl(const std::string& _url) { url = &_url; }
	inline void setBody(const std::string* jsonParamters) { body = jsonParamters; }
	inline void setHeader(const std::vector<SleepyDiscord::HeaderPair>& _header) { 
		header.reserve(_header.size() * 2);
		for (unsigned int i = 0; i < _header.size(); ++i) {
			header.push_back(_header[i].name);
			header.push_back(_header[i].value.data());
		}
	}
	inline void setMultipart(const std::initializer_list<SleepyDiscord::Part>& parts) {}
	inline SleepyDiscord::Response Post  () { return request(SleepyDiscord::Post  ); }
	inline SleepyDiscord::Response Patch () { return request(SleepyDiscord::Patch ); }
	inline SleepyDiscord::Response Delete() { return request(SleepyDiscord::Delete); }
	inline SleepyDiscord::Response Get   () { return request(SleepyDiscord::Get   ); }
	inline SleepyDiscord::Response Put   () { return request(SleepyDiscord::Put   ); }
};

//init the custom session
SleepyDiscord::CustomInit SleepyDiscord::Session::init = 
[]()->SleepyDiscord::GenericSession* { return new WebSession; };