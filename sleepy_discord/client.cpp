#if _MSC_VER && !__INTEL_COMPILER
#pragma warning( disable: 4307 )  //ignore integer overflow, becuase we are taking advantage of it
#endif

#include <chrono>
#include <functional>
#include "client.h"
#include "json.h"
#ifdef SLEEPY_USE_HARD_CODED_GATEWAY
	#include <cstring>
#endif

namespace SleepyDiscord {
	void BaseDiscordClient::start(const std::string _token, const char maxNumOfThreads) {
		ready = false;
		bot = true;
		token = std::unique_ptr<std::string>(new std::string(_token)); //add client to list
		
		messagesRemaining = 4;
		getTheGateway();
 		connect(theGateway);
#ifndef SLEEPY_ONE_THREAD
		if (2 < maxNumOfThreads) runAsync();
		if (1 < maxNumOfThreads) clock_thread = std::thread(&BaseDiscordClient::runClock_thread, this);
#endif
	}

	BaseDiscordClient::~BaseDiscordClient() {
		ready = false;
#ifndef SLEEPY_ONE_THREAD
		if (clock_thread.joinable()) clock_thread.join();
#endif
	}

	Response BaseDiscordClient::request(const RequestMethod method, const std::string _url, const std::string jsonParameters/*,
		cpr::Parameters httpParameters*/, const std::initializer_list<Part>& multipartParameters) {
		//check if rate limited
		static bool isRateLimited = false;
		static int64_t nextRetry = 0;
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
				{ "User-Agent", "SleepyDiscord (https://github.com/yourWaifu/SleepyDiscord, vtheBestVersion)" },
			};
			if (jsonParameters != "") {
				session.setBody(&jsonParameters);
				header.push_back({"Content-Type", "application/json"});
			//} else if (httpParameters.content != "") {	//this is broken for now
			//	session.SetParameters(httpParameters);
			} else if (0 < multipartParameters.size()) {
				session.setMultipart(multipartParameters);
				header.push_back({ "Content-Type", "multipart/form-data" });
			}
			header.push_back({ "Content-Length", std::to_string(jsonParameters.length()) });
			session.setHeader(header);
			//Response response;
			switch (method) {
			case Post:   response = session.Post  ();       break;
			case Patch:  response = session.Patch ();       break;
			case Delete: response = session.Delete();       break;
			case Get:    response = session.Get   ();       break;
			case Put:    response = session.Put   ();       break;
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
			std::string remaining = response.header["X-RateLimit-Remaining"];
			if (remaining != "" && std::stoi(remaining) == 0) {
				nextRetry = std::stoi(response.header["X-RateLimit-Reset"]);
				isRateLimited = true;
				setError(TOO_MANY_REQUESTS);
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
			unsigned int length = 0;
			for (const char* c = start; length == 0; ++c) {
				switch (*c) {
				case '{': start = c; break;
				case '}': length = c - start + 1; break;
				case 0: return path;
				default: break;
				}
			}
			const unsigned int startIndex = start - path.c_str();
			path.replace(startIndex, length, replaceWith);
			start = path.c_str() + startIndex + replaceWith.length() - 1;
		}
		return path;
	}

	void BaseDiscordClient::updateStatus(std::string gameName, uint64_t idleSince) {
		sendL(json::createJSON({
			{ "op", json::integer(STATUS_UPDATE) },
			{ "d", json::createJSON({
				{"idle_since", idleSince != 0? json::UInteger(idleSince) : "null"},
				{"game", gameName != "" ? json::createJSON({
					{"name", json::string(gameName)}
				}) : "null"}
			})}
		}));
	}

	void BaseDiscordClient::waitTilReady() {
		while (!ready) sleep(1000);
	}

	void BaseDiscordClient::quit() {
		disconnectWebsocket(1000);
		onQuit();
	}

#ifndef SLEEPY_ONE_THREAD
	void BaseDiscordClient::runClock_thread() {
		{
			std::mutex mut;
			std::unique_lock<std::mutex> lock(mut);
			std::condition_variable variable;
			condition = &variable;
			condition->wait(lock, [=] { return heartbeatInterval != 0; });
		}  //I don't think we need those anymore
		condition = nullptr;
		do {
			resumeHeartbeatLoop();
			sleep(heartbeatInterval);
		} while (ready);
	}
#endif

	void BaseDiscordClient::getTheGateway() {
#ifdef SLEEPY_USE_HARD_CODED_GATEWAY
		std::strncpy(theGateway, "wss://gateway.discord.gg/?v=6", 32);	//This is needed for when session is disabled
#else
		Session session;
		session.setUrl("https://discordapp.com/api/gateway");
		Response a = session.Get();	//todo change this back to a post
		if (!a.text.length()) return setError(GATEWAY_FAILED);	//error check
		//getting the gateway
		for (unsigned int position = 0, j = 0; ; ++position) {
			if (a.text[position] == '"')
				++j;
			else if (j == 3) {
				const unsigned int start = position;
				while (a.text[++position] != '"');
				unsigned int size = position - start;
				a.text.copy(theGateway, size, start);
				theGateway[size] = '/';	//super fast ultra loop unrolling
				theGateway[++size] = '?';
				theGateway[++size] = 'v';
				theGateway[++size] = '=';
				theGateway[++size] = '6';
				theGateway[++size] = 0;
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
			"\"compress\":false,"
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

	void BaseDiscordClient::reconnect(const unsigned int status) {
		if (status != 1000) {         //check for a deliberate reconnect
			heartbeatInterval = 0;    //stop heartbeating
			wasHeartbeatAcked = true; //stops the library from spamming discord
			disconnectWebsocket(status);
		}
		//loop unrolling
		if (connect(theGateway)) return;
		if (connect(theGateway)) return;
		if (connect(theGateway)) return;
		getTheGateway();
		if (connect(theGateway)) return;
		setError(CONNECT_FAILED);
	}

	void BaseDiscordClient::disconnectWebsocket(unsigned int code, const std::string reason) {
		disconnect(code, reason);
		onDisconnet();
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
			switch (hash(t.c_str())) {
			case hash("READY"): {
				Ready readyData = d;
				sessionID = readyData.sessionID;
				bot = readyData.user.bot;
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
			nextHeartbeat = getEpochTimeMillisecond();
			heartbeatInterval = std::stoi(json::getValue(d->c_str(), "heartbeat_interval"));
#ifndef SLEEPY_ONE_THREAD
			if (condition != nullptr) condition->notify_all();
			else onError(GENERAL_ERROR, "Received a HELLO packet after condition was deallocated");
#endif
			if (!ready) sendIdentity();
			else sendResume();
			break;
		case RECONNECT:
			reconnect();
			break;
		case INVALID_SESSION:
			if (d[0][0] == 't') {
				sleep(2500);
				sendResume();
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

	void BaseDiscordClient::resumeHeartbeatLoop() {
		if (!heartbeatInterval) return;

		if (nextHeartbeat <= getEpochTimeMillisecond()) {
			do nextHeartbeat += heartbeatInterval;
			while (nextHeartbeat <= getEpochTimeMillisecond()); //prevents spam

			if (!wasHeartbeatAcked) {
				reconnect(1006);
				return;
			}

			std::string str = std::to_string(lastSReceived);
			sendL("{\"op\":1,\"d\":" + str + "}");
			wasHeartbeatAcked = false;
			onHeartbeat();
		}
	}

	const int64_t BaseDiscordClient::getEpochTimeMillisecond() {
		auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		return ms.time_since_epoch().count();
	}

	const std::string BaseDiscordClient::getEditPositionString(const std::vector<std::pair<std::string, uint64_t>> positions) {
		std::vector<std::string> params(positions.size());
		for (auto value : positions) {
			params.push_back(json::createJSON({
				{ "id", json::string(value.first) },
				{ "position", json::UInteger(value.second) }
			}));
		}
		return json::createJSONArray(params);
	}
}