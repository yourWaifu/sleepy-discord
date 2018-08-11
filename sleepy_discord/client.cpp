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
		connect(theGateway, this, connection);
#ifndef SLEEPY_ONE_THREAD
		if (USE_RUN_THREAD <= maxNumOfThreads) runAsync();
#endif
	}

	BaseDiscordClient::BaseDiscordClient() : ready(false), quiting(false), bot(true), messagesRemaining(0), shardID(0), shardCount(0) {}

	BaseDiscordClient::~BaseDiscordClient() {
		ready = false;
		if (heart.isValid()) heart.stop();
	}

	Response BaseDiscordClient::request(const RequestMethod method, const std::string _url, const std::string jsonParameters/*,
		cpr::Parameters httpParameters*/, const std::initializer_list<Part>& multipartParameters) {
		//check if rate limited
		static bool isRateLimited = false;
		static time_t nextRetry = 0;
		Response response;
		if (isRateLimited) {
			if (nextRetry <= getEpochTimeMillisecond())
				isRateLimited = false;
			else {
				//Response response;
				response.statusCode = TOO_MANY_REQUESTS;
				setError(response.statusCode);
				return response;
			}
		} else if (_url == "gateway") {  //getting the gateway should happen in getTheGateway
			return { NO_CONTENT, "", {} };
		}
		{	//the { is used so that onResponse is called after session is removed to make debugging performance issues easier
			//request starts here
			Session session;
			session.setUrl("https://discordapp.com/api/v6/" + _url);
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
			case TOO_MANY_REQUESTS:   //this should fall down to default
				nextRetry = getEpochTimeMillisecond() + std::stoi(response.header["Retry-After"]);
				isRateLimited = true;
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
				const time_t resetDelta = std::mktime(resetGM) - std::mktime(&date);
				nextRetry               = (resetDelta * 1000) + getEpochTimeMillisecond();
				isRateLimited           = true;
				setError(TOO_MANY_REQUESTS);  //todo make this a warning
			}
		}
		onResponse(response);
		return response;
	}

	Response BaseDiscordClient::request(const RequestMethod method, const std::string url, const std::initializer_list<Part>& multipartParameters) {
		return request(method, url, "", /*cpr::Parameters{},*/ multipartParameters);
	}/*

	Response BaseDiscordClient::request(RequestMethod method, std::string url, cpr::Parameters httpParameters) {
		return request(method, url, "", httpParameters);
	}*/

	const std::string BaseDiscordClient::path(const char * source, std::initializer_list<std::string> values) {
		std::string path(source);
		const char* start = path.c_str();
		for (std::string replaceWith : values) {
			std::size_t length = 0;
			for (const char* c = start; length == 0; ++c) {
				switch (*c) {
				case '{': start = c; break;
				case '}': length = c - start + 1; break;
				case 0: return path;
				default: break;
				}
			}
			const std::size_t startIndex = start - path.c_str();
			path.replace(startIndex, length, replaceWith);
			start = path.c_str() + startIndex + replaceWith.length() - 1;
		}
		return path;
	}

	void BaseDiscordClient::updateStatus(std::string gameName, uint64_t idleSince, Status status, bool afk) {
		std::string statusString[] = {
			"online", "dnd", "idle", "invisible", "offline"
		};

		sendL(json::createJSON({
			{ "op", json::integer(STATUS_UPDATE) },
			{ "d", json::createJSON({
				{"idle_since", idleSince != 0 ? json::UInteger(idleSince) : "null"},
				{"game", gameName != "" ? json::createJSON({
					{"name", json::string(gameName)}
				}) : "null"},
				{ "status", SleepyDiscord::json::string(statusString[status]) },
				{ "afk", SleepyDiscord::json::boolean(afk) }
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
		theGateway = "wss://gateway.discord.gg/?v=6";	//This is needed for when session is disabled
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

	void BaseDiscordClient::quit(bool isRestarting, bool isDisconnected) {
		if (!isRestarting)
			quiting = true;

#ifdef SLEEPY_VOICE_ENABLED
		//quit all voice connections
		for (VoiceConnection& voiceConnection : voiceConnections)
			voiceConnection.disconnect();
#endif
		if (heart.isValid()) heart.stop(); //stop heartbeating
		if (!isDisconnected) disconnectWebsocket(1000);
		if (quiting) onQuit();
	}

	void BaseDiscordClient::restart() {
		quit(true);
		connect(theGateway, this, connection);
		onRestart();
	}

	void BaseDiscordClient::reconnect(const unsigned int status) {
		if (status != 1000) {         //check for a deliberate reconnect
			heartbeatInterval = 0;    //stop heartbeating
			wasHeartbeatAcked = true; //stops the library from spamming discord
		}
		disconnectWebsocket(status);
		//loop unrolling
		if (connect(theGateway, this, connection)) return;
		if (connect(theGateway, this, connection)) return;
		if (connect(theGateway, this, connection)) return;
		getTheGateway();
		if (connect(theGateway, this, connection)) return;
		setError(CONNECT_FAILED);
	}

	void BaseDiscordClient::disconnectWebsocket(unsigned int code, const std::string reason) {
		disconnect(code, reason, connection);
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
		send(message, connection);
		return true;
	}

	constexpr unsigned int hash(const char* key, unsigned int i = 0) {
		return !key[i] ? 0 : (hash(key, i + 1) * 31) + key[i] - 'A';
	}

	void BaseDiscordClient::processMessage(const std::string &message) {
		std::vector<std::string> values = json::getValues(message.c_str(),
			{ "op", "d", "s", "t" });
		int op = std::stoi(values[0]);
		const std::string &t = values[3];
		std::string *d = &values[1];
		switch (op) {
		case DISPATCH:
			lastSReceived = std::stoi(values[2]);
			switch (hash(t.c_str())) {
			case hash("READY"                      ): {
				Ready readyData = d;
				sessionID = readyData.sessionID;
				bot = readyData.user.bot;
				userID = readyData.user;
				onReady(d);
				ready = true;
				} break;
			case hash("RESUMED"                    ): onResumed           (d); break;
			case hash("GUILD_CREATE"               ): onServer            (d); break;
			case hash("GUILD_DELETE"               ): onDeleteServer      (d); break;
			case hash("GUILD_UPDATE"               ): onEditServer        (d); break;
			case hash("GUILD_BAN_ADD"              ): onBan               (d); break;
			case hash("GUILD_BAN_REMOVE"           ): onUnban             (d); break;
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
			case hash("CHANNEL_PINS_UPDATE"        ): onPinMessages       (d); break;
			case hash("PRESENCE_UPDATE"            ): onPresenceUpdate    (d); break;
			case hash("USER_UPDATE"                ): onEditUser          (d); break;
			case hash("USER_NOTE_UPDATE"           ): onEditUserNote      (d); break;
			case hash("USER_SETTINGS_UPDATE"       ): onEditUserSettings  (d); break;
			case hash("VOICE_STATE_UPDATE"         ): {
				VoiceState state(d);
#ifdef SLEEPY_VOICE_ENABLED
				if (!waitingVoiceContexts.empty()) {
					auto iterator = find_if(waitingVoiceContexts.begin(), waitingVoiceContexts.end(),
						[&state](const VoiceContext* w) { 
						return state.channelID == w->channelID && w->sessionID == "";
					});
					if (iterator != waitingVoiceContexts.end()) {
						VoiceContext& context = **iterator;
						context.sessionID = state.sessionID;
						connectToVoiceIfReady(context);
					}
				}
#endif
				onEditVoiceState(state);
				} break;
			case hash("TYPING_START"               ): onTyping            (d); break;
			case hash("MESSAGE_CREATE"             ): onMessage           (d); break;
			case hash("MESSAGE_DELETE"             ): onDeleteMessage     (d); break;
			case hash("MESSAGE_UPDATE"             ): onEditMessage       (d); break;
			case hash("MESSAGE_DELETE_BULK"        ): onBulkDelete        (d); break;
			case hash("VOICE_SERVER_UPDATE"        ): {
				VoiceServerUpdate voiceServer(d);
#ifdef SLEEPY_VOICE_ENABLED
				if (!waitingVoiceContexts.empty()) {
					auto iterator = find_if(waitingVoiceContexts.begin(), waitingVoiceContexts.end(),
						[&voiceServer](const VoiceContext* w) {
						return voiceServer.serverID == w->serverID && w->endpoint == "";
					});
					if (iterator != waitingVoiceContexts.end()) {
						VoiceContext& context = **iterator;
						context.token    = voiceServer.token;
						context.endpoint = voiceServer.endpoint;
						connectToVoiceIfReady(context);
					}
				}
#endif
				onEditVoiceServer(voiceServer);
				} break;
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
			if (!ready) sendIdentity();
			else sendResume();
			break;
		case RECONNECT:
			reconnect();
			break;
		case INVALID_SESSION:
			if (d[0][0] == 't') {
				schedule(&BaseDiscordClient::sendResume, 2500);
			} else {
				sessionID = "";
				sendIdentity();
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
		case INVALID_SEQ:
		case RATE_LIMITED:
		case SESSION_TIMEOUT:
		default:
			break;

		case 1000:
			if (!isQuiting())
				break;
			//else fall through

		//Might be Unrecoveralbe
		//We may need to stop to prevent a restart loop.
		case AUTHENTICATION_FAILED:
		case INVALID_SHARD:
		case SHARDING_REQUIRED:
			return quit(false, true);
			break;
		}
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
		std::string heartbeat;
		//The number 18 comes from 1 plus the length of {\"op\":1,\"d\":}
		heartbeat.reserve(18 + str.length());
		heartbeat += 
			"{"
				"\"op\":1,"
				"\"d\":"; heartbeat += str; heartbeat +=
			"}";
		sendL(heartbeat);
		wasHeartbeatAcked = false;
		onHeartbeat();
	}

	//
	//Voice
	//

#ifdef SLEEPY_VOICE_ENABLED

	VoiceContext& BaseDiscordClient::createVoiceContext(Snowflake<Channel> channel, Snowflake<Server> server, BaseVoiceEventHandler * eventHandler) {
		Snowflake<Server> serverTarget = server != "" ? server : getChannel(channel)->serverID;
		voiceContexts.push_front({ channel, serverTarget, eventHandler });
		waitingVoiceContexts.emplace_front(&voiceContexts.front());
		return voiceContexts.front();
	}

	void BaseDiscordClient::connectToVoiceChannel(VoiceContext& voiceContext, VoiceMode settings) {
		std::string voiceState;
		/*The number 131 came from the number of letters in this string:
		  {"op": 4,"d" : {"guild_id": "18446744073709551615",
		  "channel_id" : "18446744073709551615","self_mute" : false,"self_deaf" : false}}
		  plus one
		*/
		voiceState.reserve(131);  //remember to update this when making changes to voice status
		voiceState +=
			"{"
				"\"op\": 4,"
				"\"d\": {"
					"\"guild_id\": \""; voiceState += voiceContext.serverID; voiceState += "\","
					"\"channel_id\""": \""; voiceState += voiceContext.channelID; voiceState += "\","
					"\"self_mute\"" ": "; voiceState += settings & mute   ? "true" : "false"; voiceState += ","
					"\"self_deaf\"" ": "; voiceState += settings & deafen ? "true" : "false"; voiceState +=
				"}"
			"}";
		sendL(voiceState);
		/*Discord will response by sending a VOICE_STATE_UPDATE and a
		  VOICE_SERVER_UPDATE payload. Take a look at processMessage
		  function at case VOICE_STATE_UPDATE and voiceServerUpdate
		  */
	}

	VoiceContext& BaseDiscordClient::connectToVoiceChannel(Snowflake<Channel> channel, Snowflake<Server> server, VoiceMode settings) {
		VoiceContext& target = createVoiceContext(channel, server);
		connectToVoiceChannel(target, settings);
		return target;
	}

	void BaseDiscordClient::connectToVoiceIfReady(VoiceContext& context) {
		if (context.endpoint == "" || context.sessionID == "") //check that we are ready
			return;

		//remove the port numbers at the end of the endpoint string
		std::string& givenEndpoint = context.endpoint;
		givenEndpoint = givenEndpoint.substr(0, givenEndpoint.find(':'));

		std::string endpoint;
		//Discord doens't gives the endpoint with wss:// or ?v=3, so it's done here
		//length of wss:///?v=3 is 11, plus one equals 12
		endpoint.reserve(12 + givenEndpoint.length());
		endpoint += "wss://";
		endpoint += givenEndpoint;
		endpoint += "/?v=3";

		//Add a new connection to the list of connections
		voiceConnections.emplace_front( this, context );
		VoiceConnection& voiceConnection = voiceConnections.back();

		connect(endpoint, &voiceConnection, voiceConnection.connection);

		//remove from wait list
		waitingVoiceContexts.remove(&context);
	}

	void BaseDiscordClient::removeVoiceConnectionAndContext(VoiceConnection & connection) {
		const VoiceContext& context = connection.getContext();
		voiceConnections.remove_if(
			[&connection](VoiceConnection& right) {
				return connection == right;
			}
		);
		voiceContexts.remove(context);
	}

#endif

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
}
