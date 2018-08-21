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
		size_t namePosition;
		size_t nameLength;
		size_t valueLength;
	} JSON_findMuitipleStruct;

	void JSON_skipString(const char * JSONstring, size_t *position);
	size_t JSON_measureAndSkipString(const char* JSONstring, size_t *position);
	size_t JSON_measureString(const char * JSONstring, const size_t *_position);
	void JSON_find(const size_t numberOfNames, const char* source, JSON_findMuitipleStruct* values);
	size_t JSON_find1(const char * name, const char * source);
	void JSON_skipArray(const char * JSONstring, size_t *position);
	void JSON_skipObject(const char * JSONstring, size_t *position);
	size_t JSON_measureAndSkipObject(const char * JSONstring, size_t *position);
	size_t JSON_measureObject(const char * JSONstring, const size_t *position);
#ifdef __cplusplus
	}
#endif