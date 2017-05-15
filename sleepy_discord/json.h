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

namespace json {
	template<class Type>
	Type find(const char * name, const char * source) {
		const unsigned int startPosition = JSON_find(name, source);
		//if (startPosition == NULL) return NULL;	//what do you return in a template?
		switch (source[startPosition]) {
		case '"': {
			//const unsigned int stringLength = JSON_measureString(source, &startPosition);
			//std::string result = source + startPosition + 1;    //the + 1 will remove the " at the beginning
			//result.resize(stringLength - 1);                    //we are - 1 because we removed the "
			//if (std::is_same<Type, std::string>::value) return result;
			//else return result.c_str();
		} break;
		default:
			break;
		}
		return 0;
	}
	template<>
	std::string find<std::string>(const char * name, const char * source);

	template<class Type>   //I can't remember why this template is here
	Type find(const char * name, const std::string source) {
		const char * cString = source.c_str();
		return JSON_find(name, cString);
	}

	template<class Type>
	std::vector<Type> getArray(const std::string* _source, void(*function)(Type*, std::string)) {
		const char* source = _source->c_str();
		if (source[0] != '[') return std::vector<Type>();
		if (source[1] == ']') return std::vector<Type>();

		//get size of array and change size of target array
		const unsigned int sourceLength = _source->size();
		unsigned int arraySize = 0;
		unsigned int position = 1;
		for (; position < sourceLength; position++) {
			switch (source[position]) {
			case '"': JSON_skipString(source, &position); break;
			case '{': JSON_skipObject(source, &position); break;
			case '[': JSON_skipObject(source, &position); break;
			case ',': ++arraySize; break;
			default: break;
			}
		}
		for (bool loop = true; loop;) {	//count last variable
			switch (source[--position]) {
			case '[': return std::vector<Type>(); break;	//empty array
			case ' ': break;
			case ']': break;
			default:
				loop = false;
				++arraySize;
				break;
			}
		}
		std::vector<Type> target(arraySize);

		//fill the vector with variables
		position = 1;
		for (unsigned int index = 0; index < arraySize && position < sourceLength; position++) {	//variables should be the same type, right?
			switch (source[position]) {
			case '"': {
				const unsigned int size = JSON_measureString(source, &position) - 1;	//the -1 removes the "
				function(&(target[index++]), std::string(source + position + 1, size));
				position += size + 1;	//the +1 should skip the "
			} break;	//This should make a string, and the +1 and -1 removes the two "
			case '{': {
				const unsigned int size = JSON_measureObject(source, &position) + 1;	//the +1 adds a }
				function(&(target[index++]), std::string(source + position, size));
				position += size;
			} break;
			}
		}
		return target;
	}

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

}

#endif