#pragma once
#include <string>
#include <chrono>

struct Snowflake {
	Snowflake(std::string snow) : raw(snow) {}
	inline bool operator==(Snowflake& right) {
		return raw == right.raw;
	}
	auto timestamp() {
		if (raw == "") throw std::invalid_argument("invalid snow in Snowflake");
		return std::chrono::time_point<std::chrono::steady_clock>(std::chrono::milliseconds((std::stoll(raw) >> 22) + discordEpoch));
	}
	const std::string raw;
private:
	static const uint64_t discordEpoch = 1420070400000;	//the first second of 2015 since epoch
};