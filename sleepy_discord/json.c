#include "json.h"
/*
JSON_parseUTF16 wasn't made by me, I copied it and made some changes to make it more compatible with this JSON parser
here's the orginal code. http://llvm.org/svn/llvm-project/llvm/trunk/include/llvm/Support/ConvertUTF.h
http://llvm.org/svn/llvm-project/llvm/trunk/lib/Support/ConvertUTF.c
*/
void JSON_parseUTF16(const char *source, char *_target, size_t * position, size_t * targetPosition) {
	size_t val = 0;
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

void JSON_skipString(const char * JSONstring, size_t *position) {
	while (JSONstring[++*position] != '"') if (JSONstring[*position] == '\\')++*position;
}

size_t JSON_measureAndSkipString(const char * JSONstring, size_t *position) {
	const size_t startPosition = *position;
	JSON_skipString(JSONstring, position);
	return *position - startPosition;
}

size_t JSON_measureString(const char * JSONstring, const size_t *_position) {
	const size_t startPosition = *_position;
	size_t position = startPosition;
	JSON_skipString(JSONstring, &position);
	return position - startPosition;
}

void JSON_skipArray(const char * JSONstring, size_t *position) {
	while (JSONstring[++*position] != ']') {
		switch (JSONstring[*position]) {
		case '"': JSON_skipString(JSONstring, position); break;
		case '{': JSON_skipObject(JSONstring, position); break;
		case '[': JSON_skipArray(JSONstring, position); break;
		default: break;
		}
	}
}

void JSON_skipObject(const char * JSONstring, size_t *position) {
	while (JSONstring[++*position] != '}') {
		if (JSONstring[*position] == '"') {
			JSON_skipString(JSONstring, position);
			if (JSONstring[*position + 1] == ':') {
				for (bool loop = true; loop;) {
					loop = false;
					switch (JSONstring[++*position + 1]) {
					case '{':
						++*position;
						JSON_skipObject(JSONstring, position);
						break;
					case '[':
						++*position;
						JSON_skipArray(JSONstring, position);
						break;
					case '"':
						++*position;
						JSON_skipString(JSONstring, position);
						break;
					case 't': case 'n': *position += 4; break;
					case 'f': *position += 5; break;
					case ' ': loop = true;
					default: break;
					}
				}
			}
		}
	}
}

size_t JSON_measureAndSkipObject(const char * JSONstring, size_t *position) {
	const size_t startPosition = *position;
	if (JSONstring[*position] == '{') JSON_skipObject(JSONstring, position);
	else JSON_skipArray(JSONstring, position);
	return *position - startPosition;
}

size_t JSON_measureObject(const char * JSONstring, const size_t *position) {
	size_t startPosition = *position;
	return JSON_measureAndSkipObject(JSONstring, &startPosition);
}

void JSON_find(const size_t numberOfNames, const char* source, JSON_findMuitipleStruct* values) {
	if (*source != '{') return;

	size_t sourceLength = strlen(source);
	size_t smallestNameLength = -1;	//-1 is the largest size_t
	for (size_t i = 0; i < numberOfNames; i++) {
		if (values[i].nameLength < smallestNameLength)
			smallestNameLength = values[i].nameLength;
	}
	size_t numOfMissingValues = numberOfNames;

	if (sourceLength < smallestNameLength) {
		return; //we don't need this but it could save us some time
	}
	for (size_t position = 1; position < sourceLength; ++position) {
		switch (source[position]) {
		case '"': {
			bool found = false;				//if one of the names is the same as the string then this is true
			size_t index = 0;	//index to the variable name in names that was found
			for (; index < numberOfNames; index++) {
				//check that it hasn't been found
				if (values[index].namePosition == 0 &&
					//check that this string is on the list
					strncmp(source + position + 1, values[index].name, values[index].nameLength) == 0 &&
					//check that the string has the same length
					source[position + 1 + values[index].nameLength] == '"'
					) {
					found = true;
					JSON_skipString(source, &position);
					for (int loop = true; loop; ++position) {
						switch (source[position]) {
						case ':':
							while (source[++position] == ' ');
							values[index].namePosition = position;	//fill in the namePosition value
							switch (source[position]) {		//fill in the valueLength value
							case '"':
								values[index].valueLength = JSON_measureAndSkipString(source, &position) - 1;	//the -1 removes the " at the end
								++values[index].namePosition;													//the ++ removes the " at the beginning
								break;
							case '{': values[index].valueLength = JSON_measureAndSkipObject(source, &position) + 1; break;	//the + 1 adds the } or ] at the end
							case '[': values[index].valueLength = JSON_measureAndSkipObject(source, &position) + 1; break;
							case 'n': case 't': values[index].valueLength = 4; break;
							case 'f': values[index].valueLength = 5; break;
							case '-': case '0': case '1': case '2': case '3': case '4':         //for numbers, loop til the end of the number
							case '5': case '6': case '7': case '8': case '9': {
								size_t start = position;
								for (bool loop = true; loop;) {
									switch (source[++position]) {
									case '-': case '0': case '1': case '2': case '3': case '4':
									case '5': case '6': case '7': case '8': case '9':
										break;
									default: loop = false; break;
									}
								}
								values[index].valueLength = position - start;
								--position;       //this puts the position at the end of the number, otherwise it will cause a crash
							} break;
							default: values[index].valueLength = 1; break;													//unexpected
							}
							if (--numOfMissingValues <= 0) return;
							loop = false;
							break;
						case ',': case '}': case ']': loop = false; break;
						default: break;
						}
					}
					break;
				}
			}
			if (!found) {
				JSON_skipString(source, &position);
			}
		} break;
		case '{': JSON_skipObject(source, &position); break;
		}
	}
}

size_t JSON_find1(const char * name, const char * source) {
	JSON_findMuitipleStruct value = { name, 0, strlen(name), 0 };
	JSON_find(1, source, &value);
	return value.namePosition;
}