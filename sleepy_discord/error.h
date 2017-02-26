#pragma once

namespace SleepyDiscord {
	enum ErrorCode {
		//HTTP Response/Error Codes
		OK                  = 200, //The request completed successfully
		CREATED             = 201, //The entity was created successfully
		NO_CONTENT          = 204, //The request completed successfully but returned no content
		NOT_MODIFIED        = 304, //The entity was not modified(no action was taken)
		BAD_REQUEST         = 400, //The request was improperly formatted, or the server couldn't understand it
		UNAUTHORIZED        = 401, //The Authorization header was missing or invalid
		FORBIDDEN           = 403, //The Authorization token you passed did not have permission to the resource
		NOT_FOUND           = 404, //The resource at the location specified doesn't exist
		METHOD_NOT_ALLOWED  = 405, //The HTTP method used is not valid for the location specified
		TOO_MANY_REQUESTS   = 429, //You've made too many requests, see Rate Limiting https://discordapp.com/developers/docs/reference#rate-limiting
		GATEWAY_UNAVAILABLE = 502  //There was not a gateway available to process your request.Wait a bit and retry
		//SERVER_ERROR	    = 5xx  //The server had an error processing your request(these are rare)

		//JSON Error Response
		//I'll add them later
	};
}