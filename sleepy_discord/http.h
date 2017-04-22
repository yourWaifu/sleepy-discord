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

	class GenericSession {
	public:
		virtual void setUrl(const std::string url) = 0;
		virtual void setBody(const std::string jsonParameters) = 0;
		virtual void setHeader(std::initializer_list<std::pair<std::string, std::string>> header) = 0;
		virtual Response Post() = 0;
		virtual Response Patch() = 0;
		virtual Response Delete() = 0;
		virtual Response Get() = 0;
		virtual Response Put() = 0;
	};
};