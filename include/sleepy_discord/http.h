#pragma once
#include <string>
#include <utility>
#include <map>
#include <vector>
#include "error.h"

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

	struct Response {
		int32_t statusCode;
		std::string text;
		std::map<std::string, std::string> header;
		inline bool error() const {
			return BAD_REQUEST <= statusCode;
		}
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
		virtual void setUrl(const std::string& url) = 0;
		virtual void setBody(const std::string* jsonParameters) = 0;
		virtual void setHeader(const std::vector<HeaderPair>& header) = 0;
		virtual void setMultipart(const std::initializer_list<Part>& parts) = 0;
		virtual Response Post() = 0;
		virtual Response Patch() = 0;
		virtual Response Delete() = 0;
		virtual Response Get() = 0;
		virtual Response Put() = 0;
	protected:
		//Use this to convert RequestMethod into a string
		const char* getMethodName(const RequestMethod& method);
		
	};
};