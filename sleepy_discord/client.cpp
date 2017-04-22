#include <chrono>
#include "client.h"
#include "session.h"
#include "json.h"

namespace SleepyDiscord {
	void BaseDiscordClient::start(const std::string _token, const char maxNumOfThreads) {
#ifndef SLEEPY_ONE_THREAD
		if (1 < maxNumOfThreads) clock_thread = std::thread(&BaseDiscordClient::runClock_thread, this);
#endif
		token = std::unique_ptr<std::string>(new std::string(_token));
		getTheGateway();
 		connect(theGateway);
	}

	BaseDiscordClient::~BaseDiscordClient() {
		ready = false;
#ifndef SLEEPY_ONE_THREAD
		if (clock_thread.joinable()) clock_thread.join();
#endif
	}

	Response BaseDiscordClient::request(const RequestMethod method, std::string _url, std::string jsonParameters/*,
		cpr::Parameters httpParameters, cpr::Multipart multipartParameters*/) {
		//check if rate limited
		static bool isRateLimited = false;
		static int64_t nextRetry = 0;
		if (isRateLimited) {
			if (nextRetry <= getEpochTimeMillisecond())
				isRateLimited = false;
			else {
				Response response;
				response.statusCode = TOO_MANY_REQUESTS;
				return response;
			}
		}
		//request starts here
		Session session;
		std::pair<std::string, std::string> contentType;
		session.setUrl("https://discordapp.com/api/" + _url);
		if (jsonParameters != "") {
			session.setBody(jsonParameters);
			contentType = { "Content-Type", "application/json" };
		/*} else if (httpParameters.content != "") {	//this is broken for now
			session.SetParameters(httpParameters);
		} else if (!multipartParameters.parts.empty()) {
			session.SetMultipart(multipartParameters);
			contentType = { "Content-Type", "multipart/form-data" };*/
		}
		session.setHeader({
			{ "Authorization", "Bot " + getToken() },
			{ "User-Agent", "DiscordBot (unknown, theBestVerison)" },
			contentType,
			{ "Content-Length", std::to_string(jsonParameters.length()) }
			});
		Response response;
		switch (method) {
		case Post:   response = session.Post(); break;
		case Patch:  response = session.Patch(); break;
		case Delete: response = session.Delete(); break;
		case Get:    response = session.Get(); break;
		case Put:    response = session.Put(); break;
		default: response.statusCode = BAD_REQUEST; break;		//unexpected method
		}
		//status checking
		switch (response.statusCode) {
		case OK: case CREATED: case NO_CONTENT: case NOT_MODIFIED: break;
		case TOO_MANY_REQUESTS:   //this should fall down to default
			nextRetry = getEpochTimeMillisecond() + std::stoi(response.header["Retry-After"]);
			isRateLimited = true;
		default: {		//error
			setError(response.statusCode);		//https error
			std::vector<std::string> values = json::getValues(response.text.c_str(),
				{ "code", "message" });	//parse json to get code and message
			if (values[0] != "")
				onError(static_cast<ErrorCode>(std::stoi(values[0])), values[1]);	//send message to the function
			} break;
		}
		onResponse(response);
		return response;
	}

	/*Response BaseDiscordClient::request(RequestMethod method, std::string url, cpr::Multipart multipartParameters) {
		return request(method, url, "", cpr::Parameters{}, multipartParameters);
	}

	Response BaseDiscordClient::request(RequestMethod method, std::string url, cpr::Parameters httpParameters) {
		return request(method, url, "", httpParameters);
	}*/

	const std::string BaseDiscordClient::path(const char * source, ...) {
		va_list arguments;
		va_start(arguments, source);

		std::string path(source);
		const char* start = 0;
		for (const char* c = path.c_str(); ; ++c) {
			switch (*c) {
			case '{': start = c; break;
			case '}':
			{
				const unsigned int startIndex = start - path.c_str();
				path.replace(startIndex, c - path.c_str() - startIndex + 1, va_arg(arguments, std::string));
				c = start = path.c_str() + startIndex;
			}
			break;
			case 0:
				va_end(arguments);
				return path;
			}
		}
	}

	inline bool SleepyDiscord::BaseDiscordClient::isMagicReal() {
		return (magic[0] == 'm' && magic[1] == 'A' && magic[2] == 'g' && magic[3] == 'i' && magic[4] == 'c' && magic[5] == 0);
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
		isHeartbeatRunning = true;
		ready = false;
		waitTilReady();
		int timer = 0;
		while (ready) {
			heartbeat();
			if (timer == HALF_MINUTE_MILLISECONDS) messagesRemaining = MAX_MESSAGES_SENT_PER_HALF_MINUTE;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			--timer;
			if (timer <= 0) timer = HALF_MINUTE_MILLISECONDS;
		}
		isHeartbeatRunning = false;
	}
#endif

	bool BaseDiscordClient::updateRateLimiter(const uint8_t numOfMessages) {
		if (messagesRemaining - numOfMessages < 0) return true;
		messagesRemaining -= numOfMessages;
		return false;
	}

	void BaseDiscordClient::getTheGateway() {
		auto a = cpr::Post(cpr::Url{ "https://discordapp.com/api/gateway" });
		//getting the gateway
		for (unsigned int position = 0, j = 0; ; ++position) {
			if (a.text[position] == '"')
				++j;
			else if (j == 3) {
				const unsigned int start = position;
				while (a.text[++position] != '"');
				unsigned int size = position - start;
				a.text.copy(theGateway, size, start);
				theGateway[size] = '/';
				theGateway[++size] = '?';
				theGateway[++size] = 'v';
				theGateway[++size] = '=';
				theGateway[++size] = '6';
				theGateway[++size] = 0;
				break;
			}
		}
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
		send("{\"op\":2,\"d\":{\"token\":\"" + getToken() + "\",\"properties\":{\"$os\":\"" + os + "\",\"$browser\":\"Sleepy_Discord\",\"$device\":\"Sleepy_Discord\",\"$referrer\":\"\",\"$referring_domain\":\"\"},\"compress\":false,\"large_threshold\":250}}");
	}

	void BaseDiscordClient::sendResume() {
		send("{\"op\":6,\"d\":{\"token\":\"" + getToken() + "\",\"session_id\":\"" + session_id + "\",\"seq\":" + std::to_string(lastSReceived) + "}}");
	}

	bool BaseDiscordClient::restart() {
#ifndef SLEEPY_ONE_THREAD
		if (!isHeartbeatRunning) clock_thread = std::thread(&BaseDiscordClient::runClock_thread, this);
#endif
		return connect(theGateway);
	}

	void BaseDiscordClient::reconnect(const unsigned int status) {
		//ready = false;
		disconnectWebsocket(status);
		if (connect(theGateway)) return sendResume();
		if (connect(theGateway)) return sendResume();
		if (connect(theGateway)) return sendResume();
		getTheGateway();
		if (!connect(theGateway)) onError(OTHER, "Failed to connect to the Discord api after 4 trys");
	}

	void BaseDiscordClient::disconnectWebsocket(unsigned int code, const std::string reason) {
		disconnect(code, reason);
		onDisconnet();
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
				if (t == "READY") {
					session_id = json::getValue(d->c_str(), "session_id");
					onReady(d);
					ready = true;
				} else if (t == "GUILD_CREATE") {
					onServer(d);
				} else if (t == "MESSAGE_CREATE") {
					onMessage(d);
				} else if (t == "MESSAGE_UPDATE") {
					onEditedMessage(d);
				} else if (t == "CHANNEL_CREATE") {
					onChannel(d);
				} else if (t == "GUILD_ROLE_CREATE") {
					onEditedRole(d);
				}
				for (Event e : events) {
					if (t == e.t)
						e.function(d);
				}

				/* list of events
		READY
		RESUMED
		GUILD_CREATE
		GUILD_DELETE
		GUILD_UPDATE
		GUILD_BAN_ADD
		GUILD_BAN_REMOVE
		GUILD_MEMBER_ADD
		GUILD_MEMBER_REMOVE
		GUILD_MEMBER_UPDATE
		GUILD_ROLE_CREATE
		GUILD_ROLE_DELETE
		GUILD_ROLE_UPDATE
		GUILD_EMOJIS_UPDATE
		GUILD_MEMBERS_CHUNK
		CHANNEL_CREATE
		CHANNEL_DELETE
		CHANNEL_UPDATE
		CHANNEL_PINS_UPDATE
		PRESENCE_UPDATE
		USER_UPDATE
		USER_NOTE_UPDATE
		USER_SETTINGS_UPDATE
		VOICE_STATE_UPDATE
		TYPING_START
		MESSAGE_CREATE
		MESSAGE_DELETE
		MESSAGE_UPDATE
		MESSAGE_DELETE_BULK
		VOICE_SERVER_UPDATE
		GUILD_SYNC
		RELATIONSHIP_ADD
		RELATIONSHIP_REMOVE
		MESSAGE_REACTION_ADD
		MESSAGE_REACTION_REMOVE
		MESSAGE_REACTION_REMOVE_ALL
				*/

		break;
		case HELLO:
			heartbeatInterval = std::stoi(json::getValue(d->c_str(), "heartbeat_interval"));
			sendIdentity();
			break;
		case RECONNECT:
			reconnect();
			break;
		case INVALID_SESSION:
			sleep(2500);
			if (d[0][0] == 't') {
				sendResume();
			} else {
				session_id = "";
				sendIdentity();
			}
			break;
		case HEARTBEAT_ACK:
			wasHeartbeatAcked = true;
			onHeartbeatAck();
			break;
		}
	}

	void BaseDiscordClient::heartbeat(int op_code) {
		if (!heartbeatInterval) return;

		int64_t epochTimeMillisecond = getEpochTimeMillisecond();

		static int64_t nextHeartbeat = 0;
		if (nextHeartbeat <= epochTimeMillisecond) {	//note:this sends the heartbeat early
			if (nextHeartbeat <= 0) nextHeartbeat = epochTimeMillisecond;
			nextHeartbeat += heartbeatInterval;

			if (!wasHeartbeatAcked) {
				disconnectWebsocket(1006);
				return;
			}

			std::string str = std::to_string(lastSReceived);
			send("{\"op\":1,\"d\":" + str + "}");
			wasHeartbeatAcked = false;
			onHeartbeat();
		}
	}

	const int64_t BaseDiscordClient::getEpochTimeMillisecond() {
		auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		return ms.time_since_epoch().count();
	}
}