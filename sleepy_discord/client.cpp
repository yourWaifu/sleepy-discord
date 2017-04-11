#include "client.h"

namespace SleepyDiscord {
	void BaseDiscordClient::start(const std::string _token, const char maxNumOfThreads) {
		if (1 < maxNumOfThreads) clock_thread = std::thread(&BaseDiscordClient::runClock_thread, this);
		token = std::make_unique<std::string>(_token);
		getTheGateway();
 		connect(theGateway);	//TO-DO add v=6 support
	}

	BaseDiscordClient::~BaseDiscordClient() {
		ready = false;
		if (clock_thread.joinable()) clock_thread.join();
	}

	cpr::Response BaseDiscordClient::request(RequestMethod method, std::string _url, std::string jsonParameters,
		cpr::Parameters httpParameters, cpr::Multipart multipartParameters) {
		cpr::Session session;
		std::pair<std::string, std::string> contentType;
		session.SetUrl(cpr::Url{ "https://discordapp.com/api/" + _url });
		if (jsonParameters != "") {
			session.SetBody(cpr::Body{ jsonParameters });
			contentType = { "Content-Type", "application/json" };
		} else if (httpParameters.content != "") {
			session.SetParameters(httpParameters);
		} else if (!multipartParameters.parts.empty()) {
			session.SetMultipart(multipartParameters);
			contentType = { "Content-Type", "multipart/form-data" };
		}
		session.SetHeader(cpr::Header{
			{ "Authorization", "Bot " + getToken() },
			{ "User-Agent", "DiscordBot (unknown, theBestVerison)" },
			contentType,
			{ "Content-Length", std::to_string(jsonParameters.length()) }
			});
		updateRateLimiter();	//tells the rateLimiter clock that you've send a request
		cpr::Response response;
		switch (method) {
		case Post: response = session.Post(); break;
		case Patch: response = session.Patch(); break;
		case Delete: response = session.Delete(); break;
		case Get: response = session.Get(); break;
		case Put: response = session.Put(); break;
		default: response.status_code = BAD_REQUEST; break;		//unexpected method
		}
		switch (response.status_code) {
		case OK: case CREATED: case NO_CONTENT: case NOT_MODIFIED: break;
		default: {		//error
			setError(response.status_code);		//https error
			std::vector<std::string> values = json::getValues(response.text.c_str(),
				{ "code", "message" });	//parse json to get code and message
			if (values[0] != "")
				onError(static_cast<ErrorCode>(std::stoi(values[0])), values[1]);	//send message to the function
			} break;
		}
		return response;
	}

	cpr::Response BaseDiscordClient::request(RequestMethod method, std::string url, cpr::Multipart multipartParameters) {
		return request(method, url, "", cpr::Parameters{}, multipartParameters);
	}

	cpr::Response BaseDiscordClient::request(RequestMethod method, std::string url, cpr::Parameters httpParameters) {
		return request(method, url, "", httpParameters);
	}

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
		while (!ready) std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	void BaseDiscordClient::quit() {
		disconnectWebsocket(1000);
		onQuit();
	}

	void BaseDiscordClient::runClock_thread() {
		isHeartbeatRunning = true;
		ready = false;
		waitTilReady();
		int HalfSecondTimer = 0;
		while (ready) {
			heartbeat();
			if (HalfSecondTimer == 0) {
				//boost::this_thread::sleep(boost::posix_time::milliseconds(500));
				if (MAX_MESSAGES_SENT_PER_MINUTE <= ++rateLimiterClock)
					rateLimiterClock = 0;
				numOfMessagesSent -= rateLimiter[rateLimiterClock];
				rateLimiter[rateLimiterClock] = 0;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			++HalfSecondTimer;
			if (HalfSecondTimer == 500) HalfSecondTimer = 0;
		}
		isHeartbeatRunning = false;
	}

	void BaseDiscordClient::updateRateLimiter(const uint8_t numOfMessages) {
		rateLimiter[rateLimiterClock] += numOfMessages;
		numOfMessagesSent += numOfMessages;
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

	bool BaseDiscordClient::restart() {
		if (!isHeartbeatRunning) clock_thread = std::thread(&BaseDiscordClient::runClock_thread, this);
		return connect(theGateway);
	}

	void BaseDiscordClient::reconnect(const unsigned int status) {
		//ready = false;
		disconnectWebsocket(status);
		if (connect(theGateway)) return;
		if (connect(theGateway)) return;
		if (connect(theGateway)) return;
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
				} else if (t == "MESSAGE_CREATE") {
					onMessage(d);
				} else if (t == "MESSAGE_UPDATE") {
					onEditedMessage(d);
				} else if (t == "GUILD_CREATE") {
					onServer(d);
				} else if (t == "CHANNEL_CREATE") {
					onChannel(d);
				} else if (t == "GUILD_ROLE_CREATE") {
					onEditedRole(d);
				}
		break;
		case HELLO:
			heartbeatInterval = std::stoi(json::getValue(d->c_str(), "heartbeat_interval"));
			sendIdentity();
			break;
		case RECONNECT:
			reconnect();
			break;
		case INVALID_SESSION:
			if (d[0][0] == 't') {
				std::this_thread::sleep_for(std::chrono::milliseconds(2500));
				//send RESUME
				send("{\"op\":6,\"d\":{\"token\":\"" + getToken() + "\",\"session_id\":\"" + session_id + "\",\"seq\":" + std::to_string(lastSReceived) + "}}");
				ready = true;	//it is ready, right? I am not even sure
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

		auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		int64_t epochTimeMillisecond = ms.time_since_epoch().count();

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
}