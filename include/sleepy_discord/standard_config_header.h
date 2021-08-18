#pragma once
#ifndef SLEEPY_DO_NOT_INCLUDE_STANDARD_ONERROR
virtual void onError(SleepyDiscord::ErrorCode errorCode, const std::string errorMessage) override;
#endif

#ifndef SLEEPY_DO_NOT_INCLUDE_STANDARD_SLEEP
virtual void sleep(const unsigned int milliseconds) override;
#endif