#pragma once
#include "http.h"
#include "json.h"
#include "error.h"

namespace SleepyDiscord {
	struct StandardResponse : Response {	//this is just used for the constructor
		StandardResponse(const Response& response) : Response(response) {}
		operator const Response&() { return *this; }
	};

	struct BooleanResponse : public StandardResponse {
	private:
		typedef std::function<bool(const Response* response)> BooleanFunction;
		BooleanFunction lambdaFunction;
	public:
		BooleanResponse(const Response& response,
			BooleanFunction lambda = [](const Response* response) { return !response->error(); }
		) :
			StandardResponse(response), lambdaFunction(lambda) {}
		operator bool() const {
			return lambdaFunction(this);
		}
	};
	
	template<class Type>
	struct ObjectResponse : public StandardResponse {
		using StandardResponse::StandardResponse;
		operator Type() {
			return error() ? Type() : Type(&text);
		}

		Type& operator*() const {
			return Type();
		}

		Type* operator->() const {
			return &Type();
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