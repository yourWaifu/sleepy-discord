#include <string>
#include <vector>
#include <fstream>

//This is c++11 code, so there's no std filesystem

class File {
public:
	File(const char* path) : file(path, std::ios::binary | std::ios::ate) {
		/*std::ios::ate seeks to the end
		  This tells us where the input position indicator is
		  giving us the size                                   */
		size = file.tellg();
		/*sets the input position indicator to the begining
		  when the input position indicator reaches the end, a
		  eof flag is set and so clear() fixes that.           */
		file.clear();
		file.seekg(0, std::ios::beg);
	}

	File(std::string& path) : File(path.c_str()) {}

	~File() = default;

	template<typename Type>
	const void get(std::vector<Type>& vector) {
		const std::size_t numOfElements = size / sizeof(Type);
		vector.resize(numOfElements);
		file.read(reinterpret_cast<char*>(vector.data()), size);
	}

	template<typename Type>
	const void get(Type* buffer) {
		file.read(reinterpret_cast<char*>(buffer), size);
	}

	const std::size_t getSize() {
		return size;
	}

private:
	std::ifstream file;
	std::size_t size;
};