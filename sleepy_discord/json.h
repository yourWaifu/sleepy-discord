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

typedef enum {
	JSON_NULL = 0,
	JSON_UNDEFINED = -1,
	JSON_ERROR = -2,
	JSON_OBJECT = 1,
	JSON_ARRAY = 2,
	JSON_STRING = 3,
	JSON_NUMBER = 4,
	JSON_TRUE = 5,
	JSON_FALSE = 6
} JSON_type;

typedef struct {
	JSON_type type;
	void* value;
} JSON_value;

typedef struct {
	JSON_type type;
	void* value;
	char* name;
} JSON_pair;

typedef struct {
	unsigned int count;
	JSON_value* values;
} JSON_array;

typedef struct {
	unsigned int count;
	JSON_pair* pairs;
} JSON_object;

typedef struct {
	JSON_object* object;
	char * base;
	unsigned int size;
	unsigned int offset;
	unsigned int * objectSizes;	//array of sizes for all objects
	unsigned int * arraySizes;	//array of sizes for all arrays
	unsigned int objectNumber;	//the number for the current object
	unsigned int arrayNumber;	//The number for the current array
} JSON;

typedef struct {
	JSON_type type;
	unsigned int ID;
} ItemOnList;

typedef struct {	//values need for JSON_findMuitiple to function
	const char * name;
	unsigned int namePosition;
	unsigned int nameLength;
	unsigned int valueLength;
} JSON_findMuitipleStruct;

JSON* JSON_parseJSON(const char* JSONString, const unsigned int JSONStringSize);
JSON_object* JSON_parseObject(const char * JSONString, const unsigned int JSONStringSize, unsigned int *position, JSON* _JSON);
JSON_array* JSON_parseArray(const char * JSONString, const unsigned int JSONStringSize, unsigned int *position, JSON* _JSON);
int JSON_readFormattedInputFromString(const char *str, double * value);
void JSON_parseValue(const char * JSONString, const unsigned int JSONStringSize, unsigned int * position, JSON_type* type, void** value, JSON* _JSON);
/*
char* JSONparseString(const char * JSONString, const unsigned int JSONStringSize, unsigned int * position);
parse a string
**Parameters**
	const char * JSONString			  -	string you want to parse
	const unsigned int JSONStringSize -	number of bytes in JSONString
	unsigned int * position			  -	pointer to the position in bytes to start parsing
**Return value**
	Parsed string
**Warning**
	when using this function remember to free the returned string or you'll get a memory leak
	position will point to the 
**Example**
	std::string testingString = "\"testing string parsing\"ajdflkhjsoimnhoimjjljaordijtkl";
	unsigned int pos = 0;
	char* string = JSONparseString(testingString.c_str(), testingString.length(), &pos);
	printf(string);
	printf("\n");
	printf("&i", pos);
	free(string);
Output:
	testing string parsing

*/
char* JSON_parseString(const char * JSONstring, const unsigned int JSONstringSize, unsigned int * position, JSON* _JSON);
void JSON_parseUTF16(const char *source, char *target, unsigned int * sourcePosition, unsigned int * targetPosition);
JSON* JSON_allocateJSON(const char* source, const unsigned int sourceSize, const unsigned int* startPosition);
void* JSON_allocate(JSON * _JSON, const unsigned int size);
void JSON_deallocate(JSON* json);
bool JSON_isPairValid(JSON_pair* pair);
bool JSON_isTypeVaild(JSON_type type);
int JSON_stringCompare(char* string1, char* string2);
void* JSON_access1(JSON * json, char* string);
JSON_type JSON_accessType(JSON * json, char * string);
JSON_pair * JSON_accessPairFromObject(JSON_object * json, char * string);
void* JSON_access(JSON * json, int count, ...);
void* JSON_accessArray(JSON_array * _array, unsigned int index);
void JSON_skipString(const char * JSONstring, unsigned int *position);
unsigned int JSON_measureAndSkipString(const char* JSONstring, unsigned int *position);
unsigned int JSON_measureString(const char * JSONstring, const unsigned int *_position);
unsigned int JSON_find(const char * name, const char * source);
void JSON_findMuitiple(/*const char ** names,*/ const unsigned int numberOfNames, const char* source, JSON_findMuitipleStruct* values/*unsigned int * namePositions, const unsigned int * numeLengths*/);
unsigned int JSON_find1(const char * name, const char * source);

#ifdef __cplusplus
}

template<class Type>
Type JSON_find(const char * name, const char * source) {
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
std::string JSON_find<std::string>(const char * name, const char * source);

template<class Type>
Type JSON_find(const char * name, const std::string source) {
	const char * cString = source.c_str();
	return JSON_find<Type>(name, cString);
}

//void JSON_getValues(const char* source, const char ** names, std::string * targets, const unsigned int numOfValues);
void JSON_getValues(const char* source, const char ** names, std::string * targets, /*unsigned int * namePositions, unsigned int * sizes,*/ const unsigned int numOfValues);
void JSON_getValue(const char* source, const char * name, std::string * target);
#endif