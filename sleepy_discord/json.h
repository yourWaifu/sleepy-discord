#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
/*
TO-DO add scientific notation support
TO-DO add the varable size to JSON_array and JSON_object
TO-DO when parsing a string, make '/' use less memory
*/

#ifdef __cplusplus
#include <string>
#include <vector>
extern "C" {
#endif
	typedef struct {	//values need for JSON_findMuitiple to function
		const char * name;
		unsigned int namePosition;
		unsigned int nameLength;
		unsigned int valueLength;
	} JSON_findMuitipleStruct;

	void JSON_skipString(const char * JSONstring, unsigned int *position);
	unsigned int JSON_measureAndSkipString(const char* JSONstring, unsigned int *position);
	unsigned int JSON_measureString(const char * JSONstring, const unsigned int *_position);
	void JSON_find(const unsigned int numberOfNames, const char* source, JSON_findMuitipleStruct* values);
	unsigned int JSON_find1(const char * name, const char * source);
	void JSON_skipArray(const char * JSONstring, unsigned int *position);
	void JSON_skipObject(const char * JSONstring, unsigned int *position);
	unsigned int JSON_measureAndSkipObject(const char * JSONstring, unsigned int *position);
	unsigned int JSON_measureObject(const char * JSONstring, const unsigned int *position);
#ifdef __cplusplus
	}


namespace SleepyDiscord { namespace json {
	std::vector<std::string> getArray(const std::string* _source);
	//void JSON_getValues(const char* source, const char ** names, std::string * targets, const unsigned int numOfValues);
	std::vector<std::string> getValues(const char* source, std::initializer_list<const char *const> names);
	std::string getValue(const char* source, const char * name);
	
	const std::string createJSON(std::initializer_list<std::pair<std::string, std::string>> json);
	const std::string createJSONArray(std::vector<std::string> source);
	const std::string string(std::string s);
	const std::string UInteger(const uint64_t num);
	const std::string optionalUInteger(const uint64_t num);
	const std::string integer(const int64_t num);
	const std::string optionalInteger(const int64_t num);
	const std::string boolean(const bool boolean);

	template<class TypeToConvertTo>
	struct RawJSONArrayWrapper {
		const std::string rawJSON; //to do change this to s pointer

		RawJSONArrayWrapper(const std::string json) : rawJSON(json) {}

		template<template<class...> class Container, typename Type = TypeToConvertTo>
		Container<Type> get() {
			std::vector<std::string> jsonArray = getArray(&rawJSON);
			const unsigned int size = jsonArray.size();
			Container<Type> target(size);
			for (unsigned int i = 0; i < size; ++i)
				target[i] = &jsonArray[i];
			return target;

			//this doesn't work because the consturctors need to be using &
			//currently objects are using *
			//return Container<Type>(jsonArray.begin(), jsonArray.end());
		}
		template<template<class...> class Container>
		inline Container<std::string> getStrings() { return get<Container, std::string>(); }

		inline std::vector<std::string> vectorStrings() { return getArray(&rawJSON); }
		inline std::vector<TypeToConvertTo> vector() { return get<std::vector>(); }

		//c arrays
		inline TypeToConvertTo* cArray() { return &vector()[0]; }
		inline std::string* cArrayStrings() { return &vectorStrings()[0]; }

		//conversions
		//template<template<class...> class Container>
		//operator Container<TypeToConvertTo>() { return get<Container>(); }
		operator std::string() const { return rawJSON; }
		operator std::vector<std::string>() { return vectorStrings(); }
		operator std::vector<TypeToConvertTo>() { return vector(); }
	};
}}

#endif