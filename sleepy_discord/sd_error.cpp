#include "client.h"

namespace SleepyDiscord {
	void DiscordClient::setError(int errorCode) {
		ErrorCode code = static_cast<ErrorCode>(errorCode);
		switch (code) {
		case OK: onError(code, "The request completed successfully"); break;
		case CREATED: onError(code, "The entity was created successfully"); break;
		case NO_CONTENT: onError(code, "The request completed successfully but returned no content"); break;
		case NOT_MODIFIED: onError(code, "The entity was not modified (no action was taken)"); break;
		case BAD_REQUEST: onError(code, "The request was improperly formatted, or the server couldn't understand it"); break;
		case UNAUTHORIZED: onError(code, "The Authorization header was missing or invalid"); break;
		case FORBIDDEN: onError(code, "The Authorization token you passed did not have permission to the resource"); break;
		case NOT_FOUND: onError(code, "The resource at the location specified doesn't exist"); break;
		case METHOD_NOT_ALLOWED: onError(code, "The HTTP method used is not valid for the location specified"); break;
		case TOO_MANY_REQUESTS: onError(code, "You've made too many requests"); break;
		case GATEWAY_UNAVAILABLE: onError(code, "There was not a gateway available to process your request. Wait a bit and retry"); break;
		default:
			if (500 <= errorCode && errorCode < 600) onError(code, "The server had an error processing your request (these are rare)");
			else onError(code, "Unknown");
			break;
		}
	}
}