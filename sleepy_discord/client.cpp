#if _MSC_VER && !__INTEL_COMPILER
#pragma warning( disable: 4307 )  //ignore integer overflow, becuase we are taking advantage of it
#endif

#include <chrono>
#include <functional>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include "client.h"
#include "json.h"
#ifdef SLEEPY_USE_HARD_CODED_GATEWAY
	#include <cstring>
#endif

namespace SleepyDiscord {
	void BaseDiscordClient::start(const std::string _token, const char maxNumOfThreads, int _shardID, int _shardCount) {
		ready = false;
		quiting = false;
		bot = true;
		token = std::unique_ptr<std::string>(new std::string(_token)); //add client to list
		setShardID(_shardID, _shardCount);

		messagesRemaining = 4;
		getTheGateway();
		connect(theGateway);
#ifndef SLEEPY_ONE_THREAD
		if (3 <= maxNumOfThreads) runAsync();
#endif
	}

	BaseDiscordClient::BaseDiscordClient() : shardID(0), shardCount(0), ready(false), quiting(false), bot(true), messagesRemaining(0) {}

	BaseDiscordClient::~BaseDiscordClient() {
		ready = false;
		if (heart.isValid()) heart.stop();
	}

	Response BaseDiscordClient::request(const RequestMethod method, Route path, const std::string jsonParameters/*,
		cpr::Parameters httpParameters*/, const std::initializer_list<Part>& multipartParameters) {
		//check if rate limited
		Response response;
		const time_t currentTime = getEpochTimeMillisecond();
		if (isGlobalRateLimited) {
			if (nextRetry <= currentTime) {
				isGlobalRateLimited = false;
			} else {
				onExceededRateLimit(isGlobalRateLimited, nextRetry - currentTime, { *this, method, path, jsonParameters, multipartParameters });
				response.statusCode = TOO_MANY_REQUESTS;
				setError(response.statusCode);
				return response;
			}
		}
		const std::string bucket = path.bucket(method);
		auto bucketResetTimestamp = buckets.find(bucket);
		if (bucketResetTimestamp != buckets.end()) {
			if (bucketResetTimestamp->second <= currentTime) {
				buckets.erase(bucketResetTimestamp);
			} else {
				onExceededRateLimit(false, bucketResetTimestamp->second - currentTime, { *this, method, path, jsonParameters, multipartParameters });
				response.statusCode = TOO_MANY_REQUESTS;
				setError(response.statusCode);
				return response;
			}
		}
		{	//the { is used so that onResponse is called after session is removed to make debugging performance issues easier
			//request starts here
			Session session;
			session.setUrl("https://discordapp.com/api/v6/" + path.url());
			std::vector<HeaderPair> header = {
				{ "Authorization", bot ? "Bot " + getToken() : getToken() },
				{ "User-Agent", "DiscordBot (https://github.com/yourWaifu/SleepyDiscord, vtheBestVersion)" },
			};
			if (jsonParameters != "") {
				session.setBody(&jsonParameters);
				header.push_back({ "Content-Type"  , "application/json"                      });
				header.push_back({ "Content-Length", std::to_string(jsonParameters.length()) });
			//} else if (httpParameters.content != "") {	//this is broken for now
			//	session.SetParameters(httpParameters);
			} else if (0 < multipartParameters.size()) {
				session.setMultipart(multipartParameters);
				header.push_back({ "Content-Type", "multipart/form-data" });
			} else {
				header.push_back({ "Content-Length", "0" });
			}
			session.setHeader(header);
			//Response response;
			switch (method) {
			case Post:   response = session.Post();   break;
			case Patch:  response = session.Patch();  break;
			case Delete: response = session.Delete(); break;
			case Get:    response = session.Get();    break;
			case Put:    response = session.Put();    break;
			default:     response.statusCode = BAD_REQUEST; break; //unexpected method
			}
			//status checking
			switch (response.statusCode) {
			case OK: case CREATED: case NO_CONTENT: case NOT_MODIFIED: break;
			case TOO_MANY_REQUESTS: {   //this should fall down to default
				int retryAfter = std::stoi(response.header["Retry-After"]);
				isGlobalRateLimited = response.header["X-RateLimit-Global"] == "true";
				nextRetry = getEpochTimeMillisecond() + retryAfter;
				if (!isGlobalRateLimited)
					buckets[bucket] = nextRetry;
				onExceededRateLimit(isGlobalRateLimited, retryAfter, { *this, method, path, jsonParameters, multipartParameters });
			}
			default:
			{		//error
				const ErrorCode code = static_cast<ErrorCode>(response.statusCode);
				setError(code);		//https error
				std::vector<std::string> values = json::getValues(response.text.c_str(),
				{ "code", "message" });	//parse json to get code and message
				if (!values.empty() && values[0] != "")
					onError(static_cast<ErrorCode>(std::stoi(values[0])), values[1]);	//send message to the error event
				else
					onError(ERROR_NOTE, response.text);
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
				throw code;
#endif
			} break;
			}
			//rate limit check
			if (response.header["X-RateLimit-Remaining"] == "0" && response.statusCode != TOO_MANY_REQUESTS) {
				std::tm date = {};
				//for some reason std::get_time requires gcc 5
				std::istringstream dateStream(response.header["Date"]);
				dateStream >> std::get_time(&date, "%a, %d %b %Y %H:%M:%S GMT");
				const time_t reset = std::stoi(response.header["X-RateLimit-Reset"]);
#if defined(_WIN32) || defined(_WIN64)
				std::tm gmTM;
				std::tm*const resetGM = &gmTM;
				gmtime_s(resetGM, &reset);
#else
				std::tm* resetGM = std::gmtime(&reset);
#endif
				const time_t resetDelta = (std::mktime(resetGM) - std::mktime(&date)) * 1000;
				buckets[bucket] = resetDelta + getEpochTimeMillisecond();
				onDepletedRequestSupply(resetDelta, { *this, method, path, jsonParameters, multipartParameters });
			}
		}
		onResponse(response);
		return response;
	}

	Response BaseDiscordClient::request(const RequestMethod method, Route path, const std::initializer_list<Part>& multipartParameters) {
		return request(method, path, "", /*cpr::Parameters{},*/ multipartParameters);
	}/*

	Response BaseDiscordClient::request(RequestMethod method, std::string url, cpr::Parameters httpParameters) {
		return request(method, url, "", httpParameters);
	}*/

	const Route BaseDiscordClient::path(const char * source, std::initializer_list<std::string> values) {
		return Route(source, values);
	}

	void BaseDiscordClient::onDepletedRequestSupply(std::time_t timeTilRetry, Request request) {
	}

	void BaseDiscordClient::onExceededRateLimit(bool global, std::time_t timeTilRetry, Request request) {
		schedule(request, timeTilRetry);
	}

	void BaseDiscordClient::updateStatus(std::string gameName, uint64_t idleSince) {
		sendL(json::createJSON({
			{ "op", json::integer(STATUS_UPDATE) },
			{ "d", json::createJSON({
				{"idle_since", idleSince != 0 ? json::UInteger(idleSince) : "null"},
				{"game", gameName != "" ? json::createJSON({
					{"name", json::string(gameName)}
				}) : "null"}
			})}
		}));
	}

	void BaseDiscordClient::waitTilReady() {
		while (!ready) sleep(1000);
	}

	void BaseDiscordClient::setShardID(int _shardID, int _shardCount) {
		shardID = _shardID;
		shardCount = _shardCount;
	}

	void BaseDiscordClient::getTheGateway() {
#ifdef SLEEPY_USE_HARD_CODED_GATEWAY
		std::strncpy(theGateway, "wss://gateway.discord.gg/?v=6", 32);	//This is needed for when session is disabled
#else
		Session session;
		session.setUrl("https://discordapp.com/api/gateway");
		Response a = session.Get();	//todo change this back to a post
		if (!a.text.length()) {	//error check
			quit(false, true);
			return setError(GATEWAY_FAILED);
		}
		//getting the gateway
		for (unsigned int position = 0, j = 0; ; ++position) {
			if (a.text[position] == '"')
				++j;
			else if (j == 3) {
				const unsigned int start = position;
				while (a.text[++position] != '"');
				unsigned int size = position - start;
				theGateway.reserve(32);
				theGateway.append(a.text, start, size);
				theGateway += "/?v=6";
				break;
			}
		}
#endif
	}

	void BaseDiscordClient::sendIdentity() {
		//{
		//	"op":2,
		//	 "d":{
		//		"token":my_token,
		//		"properties":{
		//			$os":"windows 10",
		//			"$browser":"Sleepy_Discord",
		//			"$device":"Sleepy_Discord",
		//			"$referrer":"",			//I don't know what this does
		//			"$referring_domain":""		//I don't know what this does
		//		},
		//		"compress":false,
		//		"large_threshold":250			/I don't know what this does
		//	}
		//}
#if defined(_WIN32) || defined(_WIN64)
		const char * os = "Windows";
#elif defined(__APPLE__) || defined(__MACH__)
		const char * os = "macOS";
#elif defined(__linux__) || defined(linux) || defined(__linux)
		const char * os = "Linux";
#elif defined __FreeBSD__
		const char * os = "FreeBSD";
#elif defined(unix) || defined(__unix__) || defined(__unix)
		const char * os = "Unix";
#else
		const char* os = "\\u00AF\\\\_(\\u30C4)_\\/\\u00AF";  //shrug I dunno
#endif
		std::string identity;
		identity.reserve(272); //remember to change this number when editing identity
		identity += 
		"{"
			"\"op\":2,"
			"\"d\":{"
				"\"token\":\""; identity += getToken(); identity += "\","
				"\"properties\":{"
					"\"$os\":\""; identity += os; identity += "\","
					"\"$browser\":\"Sleepy_Discord\","
					"\"$device\":\"Sleepy_Discord\","
					"\"$referrer\":\"\","
					"\"$referring_domain\":\"\""
				"},"
				"\"compress\":false,";
		if (shardCount != 0 && shardID <= shardCount) {
			identity +=
				"\"shard\":[";
			identity +=	
					std::to_string(shardID); identity += ",";
			identity +=
					std::to_string(shardCount);
			identity +=
				"],";
		}
		identity +=
				"\"large_threshold\":250"
			"}"
		"}";
		sendL(identity);
	}

	void BaseDiscordClient::sendResume() {
		std::string resume;
		resume.reserve(208);
		resume +=
		"{"
			"\"op\":6,"
			"\"d\":{"
				"\"token\":\""; resume += getToken(); resume += "\","
				"\"session_id\":\""; resume += sessionID; resume += "\","
				"\"seq\":"; resume += std::to_string(lastSReceived); resume +=
			"}"
		"}";
		sendL(resume);
		onResume();
	}

//	bool BaseDiscordClient::restart() {
//#ifndef SLEEPY_ONE_THREAD
//		if (!ready) clock_thread = std::thread(&BaseDiscordClient::runClock_thread, this);
//#endif
//		return connect(theGateway);
//	}

	void BaseDiscordClient::quit(bool isRestarting, bool isDisconnected) {
		if (!isRestarting)
			quiting = true;

		if (heart.isValid()) heart.stop(); //stop heartbeating
		if (!isDisconnected) disconnectWebsocket(1000);
		if (quiting) onQuit();
	}

	void BaseDiscordClient::reconnect(const unsigned int status) {
		if (status != 1000) {         //check for a deliberate reconnect
			heartbeatInterval = 0;    //stop heartbeating
			wasHeartbeatAcked = true; //stops the library from spamming discord
		}
		disconnectWebsocket(status);
		if (consecutiveReconnectsCount == 10) getTheGateway();
		if (reconnectTimer.isValid())
			reconnectTimer.stop();
		reconnectTimer = schedule([this]() {
			connect(theGateway);
		}, consecutiveReconnectsCount < 50 ? consecutiveReconnectsCount * 5000 : 5000 * 50);
		++consecutiveReconnectsCount;
	}

	void BaseDiscordClient::disconnectWebsocket(unsigned int code, const std::string reason) {
		disconnect(code, reason);
		onDisconnect();
	}

	bool BaseDiscordClient::sendL(std::string message) {
		if (nextHalfMin <= getEpochTimeMillisecond()) {
			const unsigned int maxMessagesPerMin = 116;
			const unsigned int halfMinMilliseconds = 30000;
			const unsigned int maxMessagesPerHalfMin = maxMessagesPerMin / 2;

			nextHalfMin += halfMinMilliseconds;
			messagesRemaining = maxMessagesPerHalfMin;
		}

		if (--messagesRemaining < 0) {
			messagesRemaining = 0;
			setError(RATE_LIMITED);
			return false;
		}
		send(message);
		return true;
	}

	constexpr unsigned int hash(const char* key, unsigned int i = 0) {
		return !key[i] ? 0 : (hash(key, i + 1) * 31) + key[i] - 'A';
	}

	void BaseDiscordClient::processMessage(std::string message) {
		std::vector<std::string> values = json::getValues(message.c_str(),
			{ "op", "d", "s", "t" });
		int op = std::stoi(values[0]);
		const std::string &t = values[3];
		std::string *d = &values[1];
		switch (op) {
		case DISPATCH:
			lastSReceived = std::stoi(values[2]);
			consecutiveReconnectsCount = 0; //Successfully connected
			switch (hash(t.c_str())) {
			case hash("READY"): {
				Ready readyData = d;
				sessionID = readyData.sessionID;
				bot = readyData.user.bot;
				onReady(d);
				ready = true;
				} break;
			case hash("RESUMED"                    ): onResumed           (d); break;
			case hash("GUILD_CREATE"               ): {
				Server server(d);
				if (serverCache)
					serverCache->push_front(server);
				onServer(server);
				} break;
			case hash("GUILD_DELETE"               ): {
				UnavailableServer server(d);
				if (serverCache)
					serverCache->erase(serverCache->findServer(server));
				onDeleteServer(d);
				} break;
			case hash("GUILD_UPDATE"               ): {
				Server server(d);
				if (serverCache)
					*(serverCache->findServer(server)) = server;
				onEditServer(d);
				}break;
			case hash("GUILD_BAN_ADD"              ): onBan               (d); break;
			case hash("GUILD_BAN_REMOVE"           ): onUnban             (d); break;
			case hash("GUILD_INTEGRATIONS_UPDATE"  ):                          break; //to do add this
			case hash("GUILD_MEMBER_ADD"           ): onMember            (d); break;
			case hash("GUILD_MEMBER_REMOVE"        ): onRemoveMember      (d);
			                                          onDeleteMember      (d); break;
			case hash("GUILD_MEMBER_UPDATE"        ): onEditMember        (d); break;
			case hash("GUILD_ROLE_CREATE"          ): onRole              (d); break;
			case hash("GUILD_ROLE_DELETE"          ): onDeleteRole        (d); break;
			case hash("GUILD_ROLE_UPDATE"          ): onEditRole          (d); break;
			case hash("GUILD_EMOJIS_UPDATE"        ): onEditEmojis        (d); break;
			case hash("GUILD_MEMBERS_CHUNK"        ): onMemberChunk       (d); break;
			case hash("CHANNEL_CREATE"             ): onChannel           (d); break;
			case hash("CHANNEL_DELETE"             ): onDeleteChannel     (d); break;
			case hash("CHANNEL_UPDATE"             ): onEditChannel       (d); break;
			case hash("CHANNEL_PINS_UPDATE"        ): onPinMessage        (d); break;
			case hash("PRESENCE_UPDATE"            ): onPresenceUpdate    (d); break;
			case hash("PRESENCES_REPLACE"          ):                          break;
			case hash("USER_UPDATE"                ): onEditUser          (d); break;
			case hash("USER_NOTE_UPDATE"           ): onEditUserNote      (d); break;
			case hash("USER_SETTINGS_UPDATE"       ): onEditUserSettings  (d); break;
			case hash("VOICE_STATE_UPDATE"         ): onEditVoiceState    (d); break;
			case hash("TYPING_START"               ): onTyping            (d); break;
			case hash("MESSAGE_CREATE"             ): onMessage           (d); break;
			case hash("MESSAGE_DELETE"             ): onDeleteMessage     (d); break;
			case hash("MESSAGE_UPDATE"             ): onEditMessage       (d); break;
			case hash("MESSAGE_DELETE_BULK"        ): onBulkDelete        (d); break;
			case hash("VOICE_SERVER_UPDATE"        ): onEditVoiceServer   (d); break;
			case hash("GUILD_SYNC"                 ): onServerSync        (d); break;
			case hash("RELATIONSHIP_ADD"           ): onRelationship      (d); break;
			case hash("RELATIONSHIP_REMOVE"        ): onRemoveRelationship(d);
			                                          onDeleteRelationship(d); break;
			case hash("MESSAGE_REACTION_ADD"       ): onReaction          (d); break;
			case hash("MESSAGE_REACTION_REMOVE"    ): onRemoveReaction    (d);
			                                          onDeleteReaction    (d); break;
			case hash("MESSAGE_REACTION_REMOVE_ALL"): onRemoveAllReaction (d);
			                                          onDeleteAllReaction (d); break;
			default: setError(EVENT_UNKNOWN);                                  break;
			}
			onDispatch(d);
		break;
		case HELLO:
			heartbeatInterval = std::stoi(json::getValue(d->c_str(), "heartbeat_interval"));
			heartbeat();
			if (sessionID.empty()) sendIdentity();
			else sendResume();
			if (reconnectTimer.isValid())
				reconnectTimer.stop();
			break;
		case RECONNECT:
			reconnect();
			break;
		case INVALID_SESSION:
			if (d[0][0] == 't') {
				sleep(2500);
				sendResume();
			} else {
				sessionID = {};
				schedule(&BaseDiscordClient::sendIdentity, 2500);
			}
			break;
		case HEARTBEAT_ACK:
			wasHeartbeatAcked = true;
			onHeartbeatAck();
			break;
		}
	}

	void BaseDiscordClient::processCloseCode(const int16_t code) {
		setError(code);

		switch (code) {
		//Just reconnect
		case 1006:
		case UNKNOWN_ERROR:
		case UNKNOWN_OPCODE:
		case DECODE_ERROR:
		case NOT_AUTHENTICATED:
		case ALREADY_AUTHENTICATED:
		case RATE_LIMITED:
		case SESSION_TIMEOUT:
		default:
			break;

		case SESSION_NO_LONGER_VALID:
		case INVALID_SEQ:
		case 1000:
			if (!isQuiting()) {
				//restart with new session
				sessionID = {};
				lastSReceived = 0;
				break;
			}
			//else fall through

		//Might be Unrecoveralbe
		//We may need to stop to prevent a restart loop.
		case AUTHENTICATION_FAILED:
		case INVALID_SHARD:
		case SHARDING_REQUIRED:
			return quit(false, true);
			break;
		}
		resetHeartbeatValues();
		reconnect(1001);
	}

	void BaseDiscordClient::heartbeat() {
		if (heartbeatInterval <= 0 || isQuiting()) return; //sanity test
		
		//if time and timer are out of sync, trust time
		time_t currentTime = getEpochTimeMillisecond();
		time_t nextHeartbest;
		if (currentTime <= (nextHeartbest = lastHeartbeat + heartbeatInterval)) {
			heart = schedule(&BaseDiscordClient::heartbeat, nextHeartbest - currentTime);
			return;
		}

		if (!wasHeartbeatAcked) {
			reconnect(1001);
		} else {
			sendHeartbeat();
		}

		lastHeartbeat = currentTime;

		heart = schedule(&BaseDiscordClient::heartbeat, heartbeatInterval);
	}

	void BaseDiscordClient::sendHeartbeat() {
		std::string str = std::to_string(lastSReceived);
		sendL("{\"op\":1,\"d\":" + str + "}");
		wasHeartbeatAcked = false;
		onHeartbeat();
	}

	void BaseDiscordClient::resetHeartbeatValues() {
		if (heart.isValid()) heart.stop();
		lastHeartbeat = 0;
		wasHeartbeatAcked = true;
		heartbeatInterval = 0;
	}

	const time_t BaseDiscordClient::getEpochTimeMillisecond() {
		auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		return ms.time_since_epoch().count();
	}

	const std::string BaseDiscordClient::getEditPositionString(const std::vector<std::pair<std::string, uint64_t>>& positions) {
		std::vector<std::string> params(positions.size());
		for (auto& value : positions) {
			params.push_back(json::createJSON({
				{ "id", json::string(value.first) },
				{ "position", json::UInteger(value.second) }
			}));
		}
		return json::createJSONArray(params);
	}

	Route::Route(const std::string route, const std::initializer_list<std::string>& _values)
		: path(route), values(_values)
	{
		constexpr char channelIDIdentifier[] = "channel.id";
		constexpr char serverIDIdentifier[] = "guild.id";

		size_t targetSize = path.length();
		for (std::string replacement : values)
			targetSize += replacement.length();
		_url.reserve(targetSize);

		//In the future, use string view

		size_t offset = 0;
		for (std::string replacement : values) {
			const size_t start = path.find('{', offset);
			const size_t end = path.find('}', start);

			//the +1 and -1 removes the { and }
			const std::string identifier = path.substr(start + 1, end - start - 1);

			if (identifier == channelIDIdentifier)
				channelID = replacement;
			else if (identifier == serverIDIdentifier)
				serverID = replacement;

			_url += path.substr(offset, start - offset);
			_url += replacement;
			offset = end + 1; //the +1 removes the }
		}
		_url += path.substr(offset, path.length() - offset);

	}

	Route::Route(const char* route) : Route(route, {}) {}

	const std::string Route::bucket(RequestMethod method) {
		std::string target;
		std::string methodString = std::to_string(method);
		target.reserve(methodString.length() + channelID.string().length()
			+ serverID.string().length() + path.length());
		target += methodString;
		target += channelID;
		target += serverID;
		target += path;
		return target;
	}
}
