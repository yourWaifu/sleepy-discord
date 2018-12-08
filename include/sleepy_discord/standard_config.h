#pragma once
#ifndef SLEEPY_DO_NOT_INCLUDE_STANDARD_ONERROR
void DiscordClient::onError(SleepyDiscord::ErrorCode errorCode, const std::string errorMessage) {
	if (errorCode != 0)
		std::cout << "Error " << errorCode << ": " + errorMessage + '\n';
	else
		std::cout << "Error: " + errorMessage + '\n';
}
#endif

#ifndef SLEEPY_DO_NOT_INCLUDE_STANDARD_SLEEP
void DiscordClient::sleep(const unsigned int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
#endif