#pragma once
#include <functional>
#include "http.h"
#include "json_wrapper.h"
#include "error.h"

namespace SleepyDiscord {
	struct StandardResponse : Response {	//This is here for possiable future use
		explicit StandardResponse(const Response& response) : Response(response) {}
	};

	struct BooleanResponse : public StandardResponse {
	public:
		using StandardResponse::StandardResponse;
		using Callback = std::function<bool(const Response& response)>;
		using Type = bool;
		BooleanResponse(const Response& response, const Callback callback) :
			StandardResponse(response), wasSuccessful(callback) { }

		inline operator Type() const {
			return wasSuccessful(*this) || !error();
		}

		Type operator*() const {
			return operator Type();
		}

		inline Type cast() {
			return operator Type();
		}

		//this isn't a function so that we can override it during construction.
		//this isn't a virtual function because then we need lots of child classes
		//this isn't used in a template because then the user would have to write the right error handling function
		const Callback wasSuccessful = [](const Response& /*response*/) { return true; };
	};

	typedef BooleanResponse BoolResponse;
	
	template<ErrorCode Code>
	inline const BooleanResponse::Callback SuccessCodeFn() {
		return [](const Response& response) {return response.statusCode == Code; };
	}

	inline const BooleanResponse::Callback EmptyRespFn() {
		return SuccessCodeFn<NO_CONTENT>();
	}

	inline const BooleanResponse::Callback StandardRespFn() {
		return SuccessCodeFn<OK>();
	}


	template<class _Type>
	struct ObjectResponse : public StandardResponse {
		using StandardResponse::StandardResponse;
		using Type = _Type;

		operator Type() { //to do use references instead of pointers
			return error() ? Type() : Type(text);
		}

		Type& operator*() const {
			return operator Type();
		}

		inline Type cast() {
			return operator Type();
		}
	};


	struct ArrayResponseWrapper : public StandardResponse {
		using StandardResponse::StandardResponse;
		using Type = std::string;
		inline operator const std::string&() const {
			return text;
		}
		inline rapidjson::Document getDoc() {
			rapidjson::Document arr; //ARR, I'm a pirate
			arr.Parse(text.data(), text.length());
			return arr;
		}
	};

	template <class Type>
	using ArrayResponse = json::ArrayWrapper<Type, ArrayResponseWrapper>;

	struct StringResponse : public StandardResponse {
		using StandardResponse::StandardResponse;
		using Type = std::string;
		inline operator const Type&() const {
			return text;
		}
	};

	using VoidResponse = StringResponse;
}
