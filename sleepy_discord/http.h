#pragma once
#include <string>
#include <utility>
#include <map>

namespace SleepyDiscord {
	//http variables and functions
	//request functions
	enum RequestMethod {
		Post,
		Patch,
		Delete,
		Get,
		Put
	};

	struct Response {
		int32_t statusCode;
		std::string text;
		std::map<std::string, std::string> header;
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

	class GenericSession {
	public:
		virtual void setUrl(const std::string& url) = 0;
		virtual void setBody(const std::string* jsonParameters) = 0;
		virtual void setHeader(const std::initializer_list<std::pair<std::string, std::string>>& header) = 0;
		virtual void setMultipart(const std::initializer_list<Part>& parts) = 0;
		virtual Response Post() = 0;
		virtual Response Patch() = 0;
		virtual Response Delete() = 0;
		virtual Response Get() = 0;
		virtual Response Put() = 0;
	};
};