#include <cctype>
#include <array>
#include "http.h"

namespace SleepyDiscord {
	//copied from cpr
	bool caseInsensitiveCompare::operator()(const std::string& a, const std::string& b) const noexcept {
		return std::lexicographical_compare(
			a.begin(), a.end(), b.begin(), b.end(),
			[](unsigned char ac, unsigned char bc) { return std::tolower(ac) < std::tolower(bc); });
	}

	const char * getMethodName(const RequestMethod & method) {
		static constexpr char const * methodNames[] = { "POST", "PATCH", "DELETE", "GET", "PUT" };
		return methodNames[method];
	}

	std::string escapeURL(const std::string& string) {
		if (string.empty())
			return string;
		std::string target;
		target.reserve(string.length());

		static const auto isUrlUnresered = [](unsigned char character) {
			//copied from libcurl
			switch(character) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			case 'a': case 'b': case 'c': case 'd': case 'e':
			case 'f': case 'g': case 'h': case 'i': case 'j':
			case 'k': case 'l': case 'm': case 'n': case 'o':
			case 'p': case 'q': case 'r': case 's': case 't':
			case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
			case 'A': case 'B': case 'C': case 'D': case 'E':
			case 'F': case 'G': case 'H': case 'I': case 'J':
			case 'K': case 'L': case 'M': case 'N': case 'O':
			case 'P': case 'Q': case 'R': case 'S': case 'T':
			case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
			case '-': case '.': case '_': case '~':
				return true;
			default:
				return false;
			}
		};

		for (const auto sym : string) {
			const unsigned char character = 
				static_cast<const unsigned char>(sym);
			if (isUrlUnresered(character)) {
				target += character;
			} else {
				//encode
				static const std::array<char, 0xF + 1> hexConvert = {{
					'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
					'A', 'B', 'C', 'D', 'E', 'F'
				}};
				const std::array<char, 4> encoded {{
					'%',
					hexConvert[((0xF << 4) & sym) >> 4],
					hexConvert[0xF & sym],
					0
				}};
				target.append(encoded.data());
			}
		}
		return target;
	}
}
