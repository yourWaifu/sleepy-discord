#pragma once
#include <string>
#include <utility>
#include <map>
#include <vector>
#include <functional>
#include "error.h"

//important note, all requests on sync mode throw on an http error

namespace SleepyDiscord {
	//http variables and functions
	//request functions
	enum RequestMethod {
		Post   = 0,
		Patch  = 1,
		Delete = 2,
		Get    = 3,
		Put	   = 4
	};

	class BaseDiscordClient;

	//copied from cpr
	struct caseInsensitiveCompare {
		bool operator()(const std::string& a, const std::string& b) const noexcept;
	};

	struct Response {
		std::string text;
		int32_t statusCode = 0;
		std::map<std::string, std::string, caseInsensitiveCompare> header;
		time_t birth = 0;
		inline bool error() const {
			return BAD_REQUEST <= statusCode;
		}
		Response() = default;
		Response(int32_t _statusCode) : statusCode(_statusCode) {}
	};

	struct filePathPart {
		const std::string filePath;
	};

	struct Part {
		Part(const std::string _name, const std::string _value) :
			name(_name), value(_value), isFile(false) {}
		Part(const std::string _name, const filePathPart _file) :
			name(_name), value(_file.filePath), isFile(true) {}
		const std::string name;
		const std::string value;
		const bool isFile;   //if isFile is true then value is the filepath
	};

	typedef std::initializer_list<Part> Multipart;

	struct HeaderPair {
		const char *const name;
		std::string value = "";
		HeaderPair(const char* _name) : name(_name) {}
		HeaderPair(const char* _name, std::string _value) : name(_name), value(_value) {}
	};

	class GenericSession {
	public:
		using ResponseCallback = std::function<void(Response)>;
		virtual void setUrl(const std::string& url) = 0;
		virtual void setBody(const std::string* jsonParameters) = 0;
		virtual void setHeader(const std::vector<HeaderPair>& header) = 0;
		virtual void setMultipart(const std::vector<Part>& parts) = 0;
		virtual void setResponseCallback(const ResponseCallback& callback) = 0;
		virtual Response request(RequestMethod method) = 0;
	protected:
		//Use this to convert RequestMethod into a string
		const char* getMethodName(const RequestMethod& method);

	};
};
