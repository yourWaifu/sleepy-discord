#pragma once
#include "http.h"
#include "json.h"
#include "error.h"

namespace SleepyDiscord {
	struct StandardResponse : Response {	//this is just used for the constructor
		StandardResponse(const Response& response) : Response(response) {}
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
			return error() ? Type() : Type(&text);
		}

		inline Type cast() {
			return operator Type();
		}
	};

	struct ArrayResponseWrapper : public json::BaseArrayWrapper, public StandardResponse {
		using StandardResponse::StandardResponse;
		operator const std::string&() const {
			return text;
		}
	};

	template <class Type>
	using ArrayResponse = json::ArrayWrapper<Type, ArrayResponseWrapper>;
}
