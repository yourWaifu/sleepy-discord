#pragma once
#ifndef NONEXISTANT_GIT_INFO
	#include "version.h"
#endif
#include <unordered_set>
namespace SleepyDiscord {

	//thanks https://stackoverflow.com/a/5459929
	//convert preprocessor number into a string
	//for example:
	//#define SLEEPY_DISCORD_VERSION_BUILD 540
	//SLEEPY_DISCORD_VERSION_STR(BUILD) gives us "540"
#define SLEEPY_STR_HELPER(x) #x
#define SLEEPY_STR_HELPER2(x) SLEEPY_STR_HELPER(x)
#define SLEEPY_STR_HELPER3(x, y) x##y
#define SLEEPY_DISCORD_VERSION_STR(x) \
	SLEEPY_STR_HELPER2(SLEEPY_STR_HELPER3(SLEEPY_DISCORD_VERSION_ , x))

	//please only use defines when you want to check version via preprocessors
	//uses xxxxyyyyyy format, which can be converted to xxxx.yyyyyy
#define SLEEPY_DISCORD_VERSION_NUM 0

#if defined NONEXISTANT_VERSION_H || defined NONEXISTANT_GIT_INFO
	#define SLEEPY_DISCORD_VERSION_BUILD 0
	#define SLEEPY_DISCORD_VERSION_BRANCH "unknown branch"
	#define SLEEPY_DISCORD_VERSION_HASH "unknown revision"
	#define SLEEPY_DISCORD_VERSION_IS_MASTER 0
	//letter to use for concat description
	#define SLEEPY_DISCORD_VERSION_DESCRIPTION_CONCAT " "
	#define SLEEPY_DISCORD_VERSION_DESCRIPTION "unknown"
#endif

#define SLEEPY_DISCORD_VERSION \
	SLEEPY_DISCORD_VERSION_STR(NUM) "-"\
	SLEEPY_DISCORD_VERSION_STR(BUILD) " "\
	SLEEPY_DISCORD_VERSION_BRANCH \
	SLEEPY_DISCORD_VERSION_DESCRIPTION_CONCAT \
	SLEEPY_DISCORD_VERSION_DESCRIPTION

	constexpr unsigned int versionNum = SLEEPY_DISCORD_VERSION_NUM;
	constexpr unsigned int revisionNum = SLEEPY_DISCORD_VERSION_BUILD;
	//for some reason const fixes a warning about convering a char* to a const char*
	constexpr const char* description = SLEEPY_DISCORD_VERSION_DESCRIPTION;
	constexpr const char* branch = SLEEPY_DISCORD_VERSION_BRANCH;
	constexpr const char* revision = SLEEPY_DISCORD_VERSION_HASH;
	constexpr const char* version = SLEEPY_DISCORD_VERSION;
	constexpr bool isMaster = SLEEPY_DISCORD_VERSION_IS_MASTER;
	constexpr const char* userAgent = 
		"DiscordBot (https://github.com/yourWaifu/SleepyDiscord, " \
		SLEEPY_DISCORD_VERSION_STR(NUM) \
		") " \
		SLEEPY_DISCORD_VERSION \
	;
	
	//Features
	//Remember to list features in both preprocessers and unordered_set
#define SLEEPY_FEATURE_AVAILABLE_FEATURE_LIST
#define SLEEPY_FEATURE_LIST_OF_AVAILABLE_FEATURES
#define SLEEPY_FEATURE_V8_API
	std::unordered_set<std::string> availableFeatures{
		"Available Feature List",
		"List of Available Features",
		"V8 Discord API",
	};
	inline bool isFeatureAvaiable(std::string& featureName) {
		return availableFeatures.find(featureName) != availableFeatures.end();
	}
}
