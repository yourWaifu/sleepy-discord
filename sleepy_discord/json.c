#include "json.h"

JSON * JSON_parseJSON(const char * JSONString, const unsigned int JSONStringSize)
{
	unsigned int position = 0;
	JSON* newJSON = JSON_allocateJSON(JSONString, JSONStringSize, &position);
	if (!newJSON) {
		perror("Error allocating memory for JSON");
		abort();
	}
	if (JSONString[position] == '{') {
		JSON_object* newJSONobject;
		newJSONobject = JSON_parseObject(JSONString, JSONStringSize, &position, newJSON);
		newJSON->object = newJSONobject;
	}
	return newJSON;
}

JSON_object * JSON_parseObject(const char * JSONString, const unsigned int JSONStringSize, unsigned int * position, JSON* _JSON)
{
	JSON_object* newJSONobject = (JSON_object*)JSON_allocate(_JSON, sizeof(JSON_object));	//make the new object
	const unsigned int numOfPairs = _JSON->objectSizes[_JSON->objectNumber++];
	JSON_pair *pairs = (JSON_pair*)JSON_allocate(_JSON, sizeof(JSON_pair) * numOfPairs);
	++*position;//*position = oldPosition;
	int pairsIndex = 0;
	bool loop = true;
	while (*position < JSONStringSize && loop) {
		switch (JSONString[*position]) {
		case '\"': {	//the start of a new string
			char* string = JSON_parseString(JSONString, JSONStringSize, position, _JSON);
			while (*position < JSONStringSize) {
				if (JSONString[(*position)++] == ':') {	//if pair then parse the value
					if (numOfPairs == pairsIndex) {
						perror("Error didn't allocate enough memory to store more pairs into an object");
						abort();
					}
					pairs[pairsIndex].name = string;
					JSON_parseValue(JSONString, JSONStringSize, position, &pairs[pairsIndex].type, &pairs[pairsIndex].value, _JSON);
					++pairsIndex;
					break;
				}
			}
		}
		break;
		case ',': case '\t': case '\r': case '\n': case ' ': ++(*position); break;
		case '}':
			newJSONobject->count = numOfPairs;
			newJSONobject->pairs = pairs;
			loop = false;
			break;
		default: break;
		}
	}
	++(*position);
	return newJSONobject;
}

JSON_array * JSON_parseArray(const char * JSONString, const unsigned int JSONStringSize, unsigned int * position, JSON* _JSON)
{
	JSON_array* newJSONarray = (JSON_array*)JSON_allocate(_JSON, sizeof(JSON_array));
	const unsigned int numOfValues = _JSON->arraySizes[_JSON->arrayNumber++];
	JSON_value *values = (JSON_value*)JSON_allocate(_JSON, sizeof(JSON_value) * numOfValues);
	++*position;//*position = oldPosition;
	int valuesIndex = 0;
	bool loop = true;
	while (*position < JSONStringSize && loop) {
		switch (JSONString[*position]) {
		case ',': case '\t': case '\r': case '\n': case ' ': ++(*position); break;
		case ']':
			newJSONarray->count = numOfValues;
			newJSONarray->values = values;
			loop = false;
			break;
		default:
			if (numOfValues == valuesIndex) {
				perror("Error didn't allocate enough memory to store more values into an array");
				abort();
			}
			JSON_parseValue(JSONString, JSONStringSize, position, &values[valuesIndex].type, &values[valuesIndex].value, _JSON);
			++valuesIndex;
		}
	}
	++(*position);
	return newJSONarray;
}

int JSON_readFormattedInputFromString(const char * str, double * value)	//this is for debugging purposes
{
#ifdef _WIN32
	return sscanf_s(str, "%lf", value);
#else
	return sscanf(str, "%lf", value);	//I have no idea if this works or not
#endif
}

//JSONobject JSONparseArray() {
//
//}

void JSON_parseValue(const char * JSONString, const unsigned int JSONStringSize, unsigned int * position, JSON_type* type, void** value, JSON* _JSON) {
	int stopLoop = 0;
	while (1 > stopLoop && *position < JSONStringSize) {
		char c = tolower(JSONString[*position]);
		switch (c) {
		case '\"':
			*type = JSON_STRING;
			*value = JSON_parseString(JSONString, JSONStringSize, position, _JSON);
			break;
		case '{':
			*type = JSON_OBJECT;
			*value = JSON_parseObject(JSONString, JSONStringSize, position, _JSON);
			break;
		case '[':
			*type = JSON_ARRAY;
			*value = JSON_parseArray(JSONString, JSONStringSize, position, _JSON);
			break;
		case 't': case 'f': case 'n': {
			*type = c == 'n' ? JSON_NULL : c == 't' ? JSON_TRUE : JSON_FALSE;
			bool *boolValue = (bool *)JSON_allocate(_JSON, sizeof(bool));
			*boolValue = JSONString[*position] == 't' ? 1 : 0;
			*value = boolValue;
			*position += c == 'f' ? 5 : 4;
			break;
		}
		case '-': case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		{
			double *numValue = (double *)JSON_allocate(_JSON, sizeof(double));
			*numValue = atof(JSONString + *position);
			*type = JSON_NUMBER;
			*value = numValue;
			bool numloop = true;
			while (numloop) {
				switch (JSONString[++(*position)]) {
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9': case '.':
					break;
				/*case 'e': case 'E': {	//to do finnish this code
					bool loop = false;
					while (true) {
						switch (JSONString[++(*position)]) {
						case '+': break;
						case '-': case '0': case '1': case '2': case '3': case '4':
						case '5': case '6': case '7': case '8': case '9':

						}
					}
				}
				break;*/
				default:
					numloop = false;
				}
			}
		}
		break;
		case '\t': case '\r': case '\n': case ' ':
			--stopLoop;
			++(*position);
			break;
		default:
			*type = JSON_UNDEFINED;
			++(*position);
			break;
		}
		++stopLoop;
	}
	if (JSONStringSize < *position) {
		perror("wasn't able to find the value");
		abort();
	}
}

char * JSON_parseString(const char * JSONString, const unsigned int JSONStringSize, unsigned int * position, JSON* _JSON)
{
	unsigned int oldPosition = *position + 1;//store the old position so that we can allocate the right amount of memory
	unsigned int stringSize = JSON_measureAndSkipString(JSONString, position);;				//a string needs a size
	if (JSONStringSize < *position) {
		perror("Error string doesn't end");
		abort();
	}
	char* string = (char*)JSON_allocate(_JSON, sizeof(char) * stringSize);
	*position = oldPosition;//read comment about declearing oldPosition
	for (unsigned int newStringPosition = 0, loop = true; *position < JSONStringSize && loop; newStringPosition++) {	//copy string
		switch (JSONString[*position]) {
		case '\"':	//check if it's the end of the string, and place null at the end
			string[newStringPosition] = 0;
			++(*position);
			loop = false;
			break;
		case '\\':
			switch (JSONString[++(*position)]) {
			case '\"': string[newStringPosition] = '\"'; break;
			case '\\': string[newStringPosition] = '\\'; break;
			case '/': string[newStringPosition] = '/'; break;
			case 'b': string[newStringPosition] = '\b'; break;
			case 'f': string[newStringPosition] = '\f'; break;
			case 'n': string[newStringPosition] = '\n'; break;
			case 'r': string[newStringPosition] = '\r'; break;
			case 't': string[newStringPosition] = '\t'; break;
			case 'u': ++(*position); JSON_parseUTF16(JSONString, string + newStringPosition, position, &newStringPosition); --(*position); break;//unicode
			default: {int a = 0; a;}//Unexpected symbol
			}
			++(*position);	//you should 
			break;
		default:
			string[newStringPosition] = JSONString[(*position)++];
		}
		if (stringSize < newStringPosition) {
			perror("Error string is bigger then expected");
			abort();
		}
	}
	return string;
}

/*
JSON_parseUTF16 wasn't made by me, I copied it and made some changes to make it more compatible with this JSON parser
here's the orginal code. http://llvm.org/svn/llvm-project/llvm/trunk/include/llvm/Support/ConvertUTF.h
http://llvm.org/svn/llvm-project/llvm/trunk/lib/Support/ConvertUTF.c
*/
void JSON_parseUTF16(const char *source, char *_target, unsigned int * position, unsigned int * targetPosition) {
	unsigned int val = 0;
	const char *sourceStart = source + *position;
	char *targetStart = _target + *targetPosition;
	char c;
	for (int i = 0; i < 4; i++) {
		c = sourceStart[i];
		switch (c) {
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			val = val * 16 + c - '0';
			break;
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			val = val * 16 + c - 'a' + 10;
			break;
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			val = val * 16 + c - 'A' + 10;
		default: //Unexpected symbol
			break;
		}
		++(*position);
	}

	typedef unsigned char UTF8;   /* typically 8 bits */
	static const UTF8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
	unsigned int ch = val;
	unsigned short bytesToWrite = 0;
	const unsigned int byteMask = 0xBF;
	const unsigned int byteMark = 0x80;
	UTF8* target = (unsigned char*)targetStart;
	if (ch < 0x80)          bytesToWrite = 1; //Figure out how many bytes the result will require
	else if (ch < 0x800)    bytesToWrite = 2;
	else if (ch < 0x10000)  bytesToWrite = 3;
	else if (ch < 0x110000) bytesToWrite = 4;
	else {
		bytesToWrite = 3;
		ch = (unsigned int)0x0000FFFD;
	}
	target += bytesToWrite;
	*targetPosition += bytesToWrite - 1;
	switch (bytesToWrite) { /* note: everything falls through. */
	case 4: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
	case 3: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
	case 2: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
	case 1: *--target = (UTF8)(ch | firstByteMark[bytesToWrite]);
	}
}

JSON * JSON_allocateJSON(const char * source, const unsigned int sourceSize, const unsigned int* startPosition) {
	//
	//find the size of the whole json and allocate the json
	//
	unsigned int totalSize = 0;
	unsigned int position = *startPosition;
	unsigned int numOfObjects = 1;
	unsigned int numOfArrays = 0;
	unsigned int totalNumOfValues = 0;
	unsigned int numOfPairs = 0;
	//the start should be the { else return NULL
	if (source[*startPosition] == '{') {
		while (position < sourceSize) {
			//memory should be 8 byte aligned
			//(size + 7) & ~7 insures that totalSize will always be a multipliable of 8
			switch (source[++position]) {
			case '"': totalSize += (JSON_measureAndSkipString(source, &position) + 7) & ~7; break;
			case '[': 
				totalSize += sizeof(JSON_array);
				++numOfArrays;
				break;
			case '{': 
				totalSize += sizeof(JSON_object);
				++numOfObjects;
				break;
			case ']': if (source[position - 1] != '[') ++totalNumOfValues; break;
			case '}': if (source[position - 1] != '{') ++totalNumOfValues; break;
			case ':': 
				totalSize += (sizeof(JSON_pair) + 7) & ~7; 
				++numOfPairs;
				break;
			case 't': case 'T': case 'n': case 'N':
				totalSize += (sizeof(bool) + 7) & ~7;
				position += 3;
				break;
			case 'f': case 'F':
				totalSize += (sizeof(bool) + 7) & ~7;
				position += 4;
				break;
			case '-': case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				totalSize += sizeof(double);
				while (position < sourceSize) {
					bool loop = true;
					switch (source[position + 1]) {
					case '-': case '+': case '0': case '1': case '2': case '3': case '4':
					case '5': case '6': case '7': case '8': case '9': case '.': case 'e': case 'E':
						break;
					default: loop = false; break;
					}
					if (!loop) break;
					++position;
				}
				break;
			case ',': ++totalNumOfValues;/*if (isInsideArray) totalSize += sizeof(JSON_value);*//*if (arrayCount == 1 && !objectCount) ++valueCount;*/ break;
			default: break;
			}
		}
		totalSize += (totalNumOfValues - numOfPairs) * sizeof(JSON_value);	//gives us number of values in all arrays
		if (totalSize < sizeof(ItemOnList) * (numOfObjects + numOfArrays)) totalSize = sizeof(ItemOnList) * (numOfObjects + numOfArrays); //this is make sure that we have the space we need
		JSON* newJSON = (JSON*)malloc(sizeof(JSON) + (((sizeof(unsigned int) * (numOfObjects + numOfArrays))+7) & ~7) + totalSize);
		//
		//find the sizes of each object and array
		//
		position = *startPosition;
		unsigned int * objectSizes = (unsigned int*)(newJSON + 1);
		unsigned int * arraySizes = objectSizes + numOfObjects;
		ItemOnList * OrderedList = (ItemOnList*)(arraySizes + numOfArrays);
		unsigned int currentPointInList = 0;
		memset(objectSizes, 0, sizeof(unsigned int) * (numOfObjects + numOfArrays));
		long int objectNumber = 0;	//the number of objects counted
		long int arrayNumber = -1;	//the number of arrays counted
		long int objectIndex = 0;	//the current object that we are finding the size of
		long int arrayIndex;		//the current array that we are finding the size of
		OrderedList[0] = (ItemOnList){ .type = JSON_OBJECT, .ID = 0 };
		while (position < sourceSize) {
			switch (source[++position]) {
			case '"': JSON_skipString(source, &position); break;
			case '{': 
				++objectNumber;
				objectIndex = objectNumber;
				OrderedList[++currentPointInList].type = JSON_OBJECT;
				OrderedList[currentPointInList].ID = objectIndex;
				break;
			case '[': 
				++arrayNumber;
				arrayIndex = arrayNumber;
				OrderedList[++currentPointInList].type = JSON_ARRAY;
				OrderedList[currentPointInList].ID = arrayIndex;
				break;
			case ']':	//note: falls through to case '}'
				if (source[position - 1] != '[') ++arraySizes[arrayIndex];
			case '}':
				OrderedList[currentPointInList].type = JSON_NULL;
				OrderedList[currentPointInList].ID = 0;
				if (OrderedList[--currentPointInList].type == JSON_OBJECT) {
					objectIndex = OrderedList[currentPointInList].ID;
				} else {
					arrayIndex = OrderedList[currentPointInList].ID;
				}
				break;
			case ':': ++objectSizes[objectIndex]; break;
			case ',': if (OrderedList[currentPointInList].type == JSON_ARRAY) ++arraySizes[arrayIndex]; break;
			default: break;
			}
		}
		//
		//package everything into a JSON structure and return it
		//
		newJSON->base = (char*)(arraySizes + numOfArrays);
		if ((unsigned int)newJSON->base % 8 != 0) newJSON->base += 4;
		newJSON->size = totalSize;
		newJSON->offset = 0;
		newJSON->objectSizes = objectSizes;
		newJSON->arraySizes = arraySizes;
		newJSON->objectNumber = 0;
		newJSON->arrayNumber = 0;
		return newJSON;
	} else {
		return NULL;
	}
}

void * JSON_allocate(JSON * _JSON, const unsigned int size) {
	void* memory = _JSON->base + _JSON->offset;
	_JSON->offset += (size + 7) & ~7;	//(size + 7) & ~7 makes sure that the size is a multipliable of 8
	return memory;
}

void JSON_deallocate(JSON * json)
{
	free(json);
}

bool JSON_isPairValid(JSON_pair * pair)
{
	return JSON_isTypeVaild(pair->type);
}

bool JSON_isTypeVaild(JSON_type type) {
	switch (type) {
	case JSON_NULL: case JSON_UNDEFINED: case JSON_ERROR: case JSON_STRING:
	case JSON_NUMBER: case JSON_TRUE: case JSON_FALSE: case JSON_OBJECT: case JSON_ARRAY:
		return true;
	default:
		return false;
	}
}

int JSON_stringCompare(char* string1, char* string2)
{
	while (*string1 == *string2) {
		if (!*string1) return 0;
		++string1;
		++string2;
	}
	return *string1 < *string2 ? -1 : 1;
}

void * JSON_access1(JSON * json, char * string)
{
	JSON_pair * pair = JSON_accessPairFromObject(json->object, string);
	return pair ? pair->value : false;
}

JSON_type JSON_accessType(JSON * json, char * string)
{
	JSON_pair * pair = JSON_accessPairFromObject(json->object, string);
	return pair ? pair->type : JSON_UNDEFINED;
}

JSON_pair * JSON_accessPairFromObject(JSON_object * object, char * string)
{
	for (int i = 0; JSON_isPairValid(object->pairs + i); i++) {
		if (JSON_stringCompare(object->pairs[i].name, string) == 0) return object->pairs + i;
	}
	return false;
}

void * JSON_access(JSON * json, int count, ...) {
	va_list valistOfNames;
	va_start(valistOfNames, count);
	JSON_object * current = json->object;
	for (int i = 0; i < count; i++) {
		JSON_pair* pair = JSON_accessPairFromObject(current, va_arg(valistOfNames, char*));
		if (!pair) return false;
		if (pair->type == JSON_OBJECT) {
			current = (JSON_object *)pair->value;
		} else if (JSON_isPairValid(pair)) {
			va_end(valistOfNames);
			return pair->value;
		} else {
			va_end(valistOfNames);
			return 0;
		}
	}
	va_end(valistOfNames);
	return current;
}

void * JSON_accessArray(JSON_array * _array, unsigned int index) {
	if (JSON_isTypeVaild(_array->values[index].type)) return _array->values[index].value;
	else {
		perror("array value is invaild");
		abort();
	}
}

void JSON_skipString(const char * JSONstring, unsigned int *position) {
	while (JSONstring[++*position] != '"') if (JSONstring[*position] == '\\')++*position;
}

unsigned int JSON_measureAndSkipString(const char * JSONstring, unsigned int *position) {
	const unsigned int startPosition = *position;
	JSON_skipString(JSONstring, position);
	return *position - startPosition;
}

unsigned int JSON_measureString(const char * JSONstring, const unsigned int *_position) {
	const unsigned int startPosition = *_position;
	unsigned int position = startPosition;
	JSON_skipString(JSONstring, &position);
	return position - startPosition;
}

unsigned int JSON_find(const char * name, const char * source) {	//bug if a value that isn't a name will break this I think
	if (*source != '{') return 0;

	unsigned int sourceLength = strlen(source);
	unsigned int nameLength = strlen(name);

	if (sourceLength < nameLength) return 0;
	for (unsigned int position = 0; position < sourceLength; ++position) {
		if (source[position] == '"') {
			if (strncmp(source + position + 1, name, nameLength) == 0) {
				JSON_skipString(source, &position);
				for (int loop = true; loop; ++position) {
					switch (source[position]) {
					case ':':
						while (source[++position] == ' ');
						return position;
						break;
					case ',': case '}': loop = false; break;
					default: break;
					}
				}
			} else {
				JSON_skipString(source, &position);
			}
		}
	}
	return 0;
}
