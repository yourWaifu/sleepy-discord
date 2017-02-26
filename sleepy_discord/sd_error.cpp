#include "client.h"

namespace SleepyDiscord {
	void DiscordClient::setError(int errorCode) {
		ErrorCode code = static_cast<ErrorCode>(errorCode);
		switch (code) {
		//HTTP Response/Error Codes
		case OK:                  onError(code, "The request completed successfully"                                             ); break;
		case CREATED:             onError(code, "The entity was created successfully"                                            ); break;
		case NO_CONTENT:          onError(code, "The request completed successfully but returned no content"                     ); break;
		case NOT_MODIFIED:        onError(code, "The entity was not modified (no action was taken)"                              ); break;
		case BAD_REQUEST:         onError(code, "The request was improperly formatted, or the server couldn't understand it"     ); break;
		case UNAUTHORIZED:        onError(code, "The Authorization header was missing or invalid"                                ); break;
		case FORBIDDEN:           onError(code, "The Authorization token you passed did not have permission to the resource"     ); break;
		case NOT_FOUND:           onError(code, "The resource at the location specified doesn't exist"                           ); break;
		case METHOD_NOT_ALLOWED:  onError(code, "The HTTP method used is not valid for the location specified"                   ); break;
		case TOO_MANY_REQUESTS:   onError(code, "You've made too many requests"                                                  ); break;
		case GATEWAY_UNAVAILABLE: onError(code, "There was not a gateway available to process your request. Wait a bit and retry"); break;
		default:
			if (500 <= errorCode && errorCode < 600) onError(code, "The server had an error processing your request (these are rare)");
			else onError(code, "Unknown");
			break;

		//JSON Error Response
		case 10001: onError(code, "Unknown account"                                                                                               ); break;
		case 10002: onError(code, "Unknown application"                                                                                           ); break;
		case 10003: onError(code, "Unknown channel"                                                                                               ); break;
		case 10004: onError(code, "Unknown guild"                                                                                                 ); break;
		case 10005: onError(code, "Unknown integration"                                                                                           ); break;
		case 10006: onError(code, "Unknown invite"                                                                                                ); break;
		case 10007: onError(code, "Unknown member"                                                                                                ); break;
		case 10008: onError(code, "Unknown message"                                                                                               ); break;
		case 10009: onError(code, "Unknown overwrite"                                                                                             ); break;
		case 10010: onError(code, "Unknown provider"                                                                                              ); break;
		case 10011: onError(code, "Unknown role"                                                                                                  ); break;
		case 10012: onError(code, "Unknown token"                                                                                                 ); break;
		case 10013: onError(code, "Unknown user"                                                                                                  ); break;
		case 10014: onError(code, "Unknown Emoji"                                                                                                 ); break;
		case 20001: onError(code, "Bots cannot use this endpoint"                                                                                 ); break;
		case 20002: onError(code, "Only bots can use this endpoint"                                                                               ); break;
		case 30001: onError(code, "Maximum number of guilds reached(100)"                                                                         ); break;
		case 30002: onError(code, "Maximum number of friends reached(1000)"                                                                       ); break;
		case 30003: onError(code, "Maximum number of pins reached(50)"                                                                            ); break;
		case 30005: onError(code, "Maximum number of guild roles reached(250)"                                                                    ); break;
		case 30010: onError(code, "Too many reactions"                                                                                            ); break;
		case 40001: onError(code, "Unauthorized"                                                                                                  ); break;
		case 50001: onError(code, "Missing access"                                                                                                ); break;
		case 50002: onError(code, "Invalid account type"                                                                                          ); break;
		case 50003: onError(code, "Cannot execute action on a DM channel"                                                                         ); break;
		case 50004: onError(code, "Embed disabled"                                                                                                ); break;
		case 50005: onError(code, "Cannot edit a message authored by another user"                                                                ); break;
		case 50006: onError(code, "Cannot send an empty message"                                                                                  ); break;
		case 50007: onError(code, "Cannot send messages to this user"                                                                             ); break;
		case 50008: onError(code, "Cannot send messages in a voice channel"                                                                       ); break;
		case 50009: onError(code, "Channel verification level is too high"                                                                        ); break;
		case 50010: onError(code, "OAuth2 application does not have a bot"                                                                        ); break;
		case 50011: onError(code, "OAuth2 application limit reached"                                                                              ); break;
		case 50012: onError(code, "Invalid OAuth state"                                                                                           ); break;
		case 50013: onError(code, "Missing permissions"                                                                                           ); break;
		case 50014: onError(code, "Invalid authentication token"                                                                                  ); break;
		case 50015: onError(code, "Note is too long"                                                                                              ); break;
		case 50016: onError(code, "Provided too few or too many messages to delete.Must provide at least 2 and fewer than 100 messages to delete."); break;
		case 50019: onError(code, "A message can only be pinned to the channel it was sent in"                                                    ); break;
		case 50034: onError(code, "A message provided was too old to bulk delete"                                                                 ); break;
		case 90001: onError(code, "Reaction Blocked"                                                                                              ); break;
		}																															   
	}
}