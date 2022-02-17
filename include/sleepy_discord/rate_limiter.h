#pragma once
#include "client.h"

namespace SleepyDiscord {

	class Route {
	public:
		using Bucket = std::string;
		Route(const std::string route, const std::initializer_list<std::string>& _values = {});
		Route(const char* route);
		inline const std::string& url() {
			return _url;
		}
		const Bucket bucket(RequestMethod method);
		inline operator const std::string&() {
			return url();
		}
		inline const std::string& getPath() {
			return path;
		}

	private:
		const std::string path;
		std::string _url;
		const std::initializer_list<std::string>& values;

		//for the snowflake part, discord class should do
		std::unordered_map<std::string, Snowflake<User>::RawType>
			majorParameters = {
			{ "channel.id", {} },
			{ "guild.id"  , {} },
			{ "webhook.id", {} }
		};
	};

	//note: all rate limiter data needs to be handled in a sync manner
	template<class Client>
	struct RateLimiter {
		std::atomic<bool> isGlobalRateLimited = { false };
		std::atomic<time_t> nextRetry = { 0 };
		void limitBucket(const Route::Bucket& bucket, const std::string& xBucket, time_t timestamp)  {
			std::lock_guard<std::mutex> lock(mutex);
			buckets[bucket] = xBucket;
			limits[xBucket].nextTry = timestamp;
		}
		
		const time_t getLiftTime(Route::Bucket& bucket, const time_t& currentTime) {
			if (isGlobalRateLimited && currentTime < nextRetry)
					return nextRetry;
			isGlobalRateLimited = false;
			std::lock_guard<std::mutex> lock(mutex);
			auto actualBucket = buckets.find(bucket);
			if (actualBucket != buckets.end()) {
				auto rateLimit = limits.find(actualBucket->second);
				if (rateLimit != limits.end()) {
					if (currentTime < rateLimit->second.nextTry)
						return rateLimit->second.nextTry;
					limits.erase(rateLimit);
				}
				buckets.erase(actualBucket);
			}
			return 0;
		}
		//isLimited also returns the next Retry timestamp

		struct RateLimit {
		public:
			void doWaitingRequest()  {
				if (awaitingRequest.empty())
					return;
				RateLimiter& rateLimiter = awaitingRequest.front().client.rateLimiter;

				std::lock_guard<std::mutex> lock(rateLimiter.mutex);
				awaitingRequest.remove_if([](typename Client::Request& request){
					request.client.postTask(request);
					return true;
				});
			}
		private:
			friend RateLimiter;
			std::list<typename Client::Request> awaitingRequest;
			time_t nextTry = 0; // to do for v8, make this seconds
			static constexpr int defaultLimit = 1;
			int limit = defaultLimit;
			int remaining = defaultLimit;
			Timer expireTimer;
		};

		class HandleAwaitAfterRequest {
		public:
			HandleAwaitAfterRequest(RateLimiter::RateLimit& limit)
				: rateLimit(limit) {}
			~HandleAwaitAfterRequest() {
				rateLimit.doWaitingRequest();
			}
		private:
			RateLimiter::RateLimit& rateLimit;
		};

	private:
		std::unordered_map<Route::Bucket, std::string> buckets;
		std::unordered_map<std::string, RateLimit> limits;
		std::mutex mutex;
	};
}