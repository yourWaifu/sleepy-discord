#pragma once
#include <functional>
#include "http.h"
#include "json_wrapper.h"
#include "error.h"

namespace SleepyDiscord {
	struct StandardResponse : Response {	//This is here for possiable future use
		StandardResponse(const Response& response) : Response(response) {}
	};

	struct BooleanResponse : public StandardResponse {
	public:
		using StandardResponse::StandardResponse;
		using Callback = std::function<bool(const Response& response)>;
		BooleanResponse(const Response& response, const Callback callback) :
			StandardResponse(response), wasSuccessful(callback) { }

		inline operator bool() const {
			return wasSuccessful(*this) || !error();
		}
		
		bool operator*() const {
			return operator bool();
		}

		inline bool cast() {
			return operator bool();
		}

		const Callback wasSuccessful = [](const Response& response) { return true; };
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


	template<class Type>
	struct ObjectResponse : public StandardResponse {
		using StandardResponse::StandardResponse;
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


	struct ArrayResponseWrapper : public json::BaseArrayWrapper, public StandardResponse {
		using StandardResponse::StandardResponse;
		inline operator const std::string&() const {
			return text;
		}
		inline json::Array getArray() override {
			rapidjson::Document arr; //I'm a pirate
			arr.Parse(text.data(), text.length());
			return arr.Get<rapidjson::Document::ConstArray>();
		}
	};

	template <class Type>
	using ArrayResponse = json::ArrayWrapper<Type, ArrayResponseWrapper>;

	struct StringResponse : public StandardResponse {
		using StandardResponse::StandardResponse;
		inline operator const std::string&() const {
			return text;
		}
	};

	using VoidResponse = StringResponse;
}