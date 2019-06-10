#include <string>
#include <cstdio>
#include <vector>

//This is c++11 code, so there's no std filesystem

class File {
public:
	File(const char* path) {
#ifdef _WIN32
		fopen_s(&fileHandle, path, "rb")
#else
		fileHandle = fopen(path, "rb");
#endif
		if (fileHandle == nullptr) {
			size = -1;
			return;
		}
		//get file length
		fseek(fileHandle, 0L, SEEK_END);
		size = ftell(fileHandle);
		rewind(fileHandle);
	}

	File(std::string& path) : File(path.c_str()) {}

	~File() {
		if (fileHandle != nullptr)
			fclose(fileHandle);
	}

	template<typename Type>
	const void get(std::vector<Type>& vector) {
		const std::size_t numOfElements = size / sizeof(Type);
		vector.resize(numOfElements);
		fread(vector.data(), sizeof(Type), numOfElements, fileHandle);
	}

	template<typename Type>
	const void get(Type* buffer) {
		fread(buffer, sizeof(Type), size / sizeof(Type), fileHandle);
	}

	const std::size_t getSize() {
		return size;
	}

private:
	FILE * fileHandle = nullptr;
	std::size_t size;
};