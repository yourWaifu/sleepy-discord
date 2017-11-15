#pragma once
#include "http.h"
#include "json.h"
#include "error.h"

namespace SleepyDiscord {
	struct StandardResponse : Response {	//this is just used for the constructor
		StandardResponse(Response& response) : Response(response) {}
		operator const Response&() { return *this; }
	};

	template<ErrorCode successCode>
	struct BooleanResponse : public StandardResponse {
		using StandardResponse::StandardResponse;
		operator bool() {
			return statusCode == successCode;
		}
	};
	
	template<class Type>
	struct ObjectResponse : public StandardResponse {
		using StandardResponse::StandardResponse;
		operator Type() {
			if (!error()) return Type(text);
			else return Type();
		}
	};

	struct ArrayResponseWrapper : public json::BaseArrayWrapper, public StandardResponse {
		using StandardResponse::StandardResponse;
		inline std::vector<std::string> vectorStrings() { return json::getArray(&text); }

		operator const std::string&() const { return text; }
	};

	template <class Type>
	using ArrayResponse = json::ArrayWrapper<Type, ArrayResponseWrapper>;
}