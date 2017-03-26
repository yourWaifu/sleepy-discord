#include "client.h"

namespace SleepyDiscord {
	void DiscordClient::start(const std::string _token) {
		clock_thread = std::thread(&DiscordClient::runClock_thread, this);
		token = std::make_unique<std::string>(_token);
		getTheGateway();
 		connect(theGateway);	//TO-DO add v=6 support
	}

	DiscordClient::~DiscordClient() {
		ready = false;
	}

	cpr::Response DiscordClient::request(RequestMethod method, std::string _url, std::string jsonParameters,
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

	cpr::Response DiscordClient::request(RequestMethod method, std::string url, cpr::Multipart multipartParameters) {
		return request(method, url, "", cpr::Parameters{}, multipartParameters);
	}

	cpr::Response DiscordClient::request(RequestMethod method, std::string url, cpr::Parameters httpParameters) {
		return request(method, url, "", httpParameters);
	}

	const std::string DiscordClient::path(const char * source, ...) {
		va_list arguments;
		va_start(arguments, source);
		
		std::string path(source);
		unsigned int start = 0;
		for (const char* c = source; ; ++c) {
			switch (*c) {
			case '{': start = c - source; break;
			case '}': path.replace(start, c - source - start + 1, va_arg(arguments, const char *)); break;
			case 0: 
				va_end(arguments);
				return path;
			}
		}
	}

	inline bool SleepyDiscord::DiscordClient::isMagicReal() {
		return (magic[0] == 'm' && magic[1] == 'A' && magic[2] == 'g' && magic[3] == 'i' && magic[4] == 'c' && magic[5] == 0);
	}

	Message DiscordClient::sendMessage(std::string channel_id, std::string message, bool tts) {
		auto r = request(Post, path("channels/{channel.id}/messages", channel_id), "{\"content\":\"" + message + (tts?"\",\"tts\":\"true\"":"\"") +"}");
		return Message(&r.text);
	}

	Message DiscordClient::uploadFile(std::string channel_id, std::string fileLocation, std::string message) {
		auto r = request(Post, "channels/" + channel_id + "/messages", 
		                 cpr::Multipart{ { "content", message },
		                                 {"file", cpr::File{fileLocation} }
		                 }
		);
		return Message(&r.text);
	}

	Message DiscordClient::editMessage(std::string channel_id, std::string message_id, std::string newMessage) {
		auto r = request(Patch, "channels/" + channel_id + "/messages/" + message_id, "{\"content\": \"" + newMessage + "\"}");
		return Message(&r.text);
	}

	bool DiscordClient::deleteMessage(const std::string channel_id, const std::string * message_id, const unsigned int numOfMessages) {
		if (numOfMessages == 1) return request(Delete, "channels/" + channel_id + "/messages/" + *message_id).status_code == NO_CONTENT;
		else {
			std::string JSON = "{\"messages\":[";
			for (unsigned int i = 0; i < numOfMessages; ++i) {
				JSON += message_id[i];
			}
			JSON += "]}";
			return request(Delete, "channels/" + channel_id + "/messages/" + *message_id).status_code == NO_CONTENT;
		}
	}

	Channel DiscordClient::createTextChannel(std::string server_id, std::string name) {
		auto r = request(Post, "guilds/" + server_id + "/channels", "{\"name\": \"" + name + "\", \"type\": \"text\"}");
		return Channel(&r.text);
	}
	
	Channel DiscordClient::editChannel(std::string channel_id, std::string name, std::string topic) {
		std::string json = "{";
		if (name != "")
			json += "\"name\":\"" + name + "\",";
		if (topic != "")
			json += "\"topic\":\"" + topic + "\",";
		json[json.length() - 1] = '}';
		auto r = request(Patch, "channels/" + channel_id, json);
		return Channel(&r.text);
	}

	Channel DiscordClient::editChannelName(std::string channel_id, std::string name) {
		return editChannel(channel_id, name);
	}

	Channel DiscordClient::editChannelTopic(std::string channel_id, std::string topic) {
		return editChannel(channel_id, "", topic);
	}

	Channel DiscordClient::deleteChannel(std::string channel_id) {
		auto r = request(Delete, "channels/" + channel_id);
		return Channel(&r.text);
	}

	void DiscordClient::testFunction(std::string teststring) {
		disconnect(1001, "");
		connect(theGateway);
	}

	Channel DiscordClient::getChannel(std::string channel_id) {
		std::string r = request(Get, "channels/" + channel_id).text;
		return Channel(&r);
	}

	Message DiscordClient::getMessage(std::string channel_id, std::string message_id) {
		std::string r = request(Get, "channels/" + channel_id + "/messages/ + message_id").text;
		return Message(&r);
	}

	bool DiscordClient::addReaction(std::string channel_id, std::string message_id, std::string emoji) {
		return request(Put, "channels/" + channel_id + "/messages/" + message_id + "/reactions/" + emoji +"/@me").status_code == NO_CONTENT;
	}

	bool DiscordClient::removeReaction(std::string channel_id, std::string message_id, std::string emoji, std::string user_id) {
		return request(Put, "channels/" + channel_id + "/messages/" + message_id + "/reactions/" + emoji + '/' + user_id).status_code == NO_CONTENT;
	}

	bool DiscordClient::sendTyping(std::string channel_id) {
		return request(Post, "channels/" + channel_id + "/typing").status_code == NO_CONTENT;
	}

	bool DiscordClient::pinMessage(std::string channel_id, std::string message_id) {
		return request(Put, "channels/" + channel_id + "/pins/" + message_id).status_code == NO_CONTENT;
	}

	bool DiscordClient::unpinMessage(std::string channel_id, std::string message_id) {
		return request(Delete, "channels/" + channel_id + "/pins/" + message_id).status_code == NO_CONTENT;
	}

	int SleepyDiscord::DiscordClient::createRole(std::string server_id) {
		auto r = request(Post, "guilds/" + server_id + "/roles");
		return r.status_code;
	}

	bool SleepyDiscord::DiscordClient::deleteRole(std::string server_id, std::string role_id) {
		return request(Delete, "guilds/" + server_id + "/roles/" + role_id).status_code == NO_CONTENT;
	}

	bool DiscordClient::muteServerMember(std::string server_id, std::string user_id, bool mute) {
		return request(Patch, "guilds/" + server_id + "/members/" + user_id, mute ? "{\"mute\":true}" : "{\"mute\":false}").status_code == NO_CONTENT;
	}

	Server DiscordClient::getServer(std::string server_id) {
		auto r = request(Get, "guilds/" + server_id);
		return Server(&r.text);
	}

	Server DiscordClient::deleteServer(std::string server_id) {
		auto r = request(Delete, "guilds/" + server_id);
		return Server(&r.text);
	}

	bool DiscordClient::editNickname(std::string server_id, std::string newNickname) {
		return request(Patch, "guilds/"+ server_id + "/members/@me/nick", "{\"nick\":\"" + newNickname + "\"}").status_code == OK;
	}

	bool DiscordClient::addRole(std::string server_id, std::string member_id, std::string role_id) {
		return request(Put, "guilds/" + server_id + "/members/" + member_id + "/roles/" + role_id).status_code == NO_CONTENT;
	}

	bool DiscordClient::removeRole(std::string server_id, std::string member_id, std::string role_id) {
		return request(Delete, "guilds/" + server_id + "/members/" + member_id + "/roles/" + role_id).status_code == NO_CONTENT;
	}

	bool DiscordClient::kickMember(std::string server_id, std::string member_id) {
		return request(Delete, "guilds/" + server_id + "/members/" + member_id).status_code == NO_CONTENT;
	}

	bool DiscordClient::banMember(std::string server_id, std::string member_id) {
		return request(Put, "guilds/" + server_id + "/bans/" + member_id).status_code == NO_CONTENT;
	}

	bool DiscordClient::unbanMember(std::string server_id, std::string member_id) {
		return request(Delete, "guilds/" + server_id + "/bans/" + member_id).status_code == NO_CONTENT;
	}

	void DiscordClient::pruneMembers(std::string server_id, const unsigned int numOfDays) {
		if (numOfDays == 0) return;
		request(Post, "guilds/" + server_id + "/prune", "{\"days\":" + numOfDays + '}');
	}

	bool DiscordClient::deleteIntegration(std::string server_id, std::string integration_id) {
		return request(Delete, "guilds/" + server_id + "/integrations/" + integration_id).status_code == NO_CONTENT;
	}

	bool DiscordClient::syncIntegration(std::string server_id, std::string integration_id) {
		return request(Post, "guilds/" + server_id + "/integrations/" + integration_id + "/sync").status_code == NO_CONTENT;
	}

	Invite DiscordClient::inviteEndpoint(RequestMethod method, std::string inviteCode) {
		auto r = request(method, "invites/" + inviteCode);
		return Invite(&r.text);
	}

	Invite DiscordClient::getInvite(std::string inviteCode) {
		return inviteEndpoint(Get, inviteCode);
	}

	Invite DiscordClient::deleteInvite(std::string inviteCode) {
		return inviteEndpoint(Delete, inviteCode);
	}

	Invite DiscordClient::acceptInvite(std::string inviteCode) {
		return inviteEndpoint(Post, inviteCode);
	}

	User DiscordClient::getCurrentUser() {
		auto r = request(Get, "users/@me");
		return User(&r.text);
	}

	User DiscordClient::getUser(std::string user_id) {
		auto r = request(Get, "users/" + user_id);
		return User(&r.text);
	}

	bool DiscordClient::leaveServer(std::string server_id) {
		return request(Delete, "users/@me/guilds/" + server_id).status_code == NO_CONTENT;
	}

	void DiscordClient::waitTilReady() {
		while (!ready) std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	void DiscordClient::runClock_thread() {
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
	}

	void DiscordClient::updateRateLimiter(const uint8_t numOfMessages) {
		rateLimiter[rateLimiterClock] += numOfMessages;
		numOfMessagesSent += numOfMessages;
	}

	void DiscordClient::getTheGateway() {
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

	void DiscordClient::sendIdentity() {
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
		send("{\"op\":2,\"d\":{\"token\":\"" + getToken() + "\",\"properties\":{\"$os\":\"windows 10\",\"$browser\":\"Sleepy_Discord\",\"$device\":\"Sleepy_Discord\",\"$referrer\":\"\",\"$referring_domain\":\"\"},\"compress\":false,\"large_threshold\":250}}");
	}

	void DiscordClient::processMessage(std::string message) {
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
			ready = false;
			disconnect(1000, "");
			if (connect(theGateway)) break;
			if (connect(theGateway)) break;
			if (connect(theGateway)) break;
			getTheGateway();
			if (!connect(theGateway)) onError(OTHER, "Failed to connect to the Discord api after 4 trys");
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
			onHeartbeatAck();
			break;
		}
	}

	void DiscordClient::heartbeat(int op_code) {
		if (!heartbeatInterval) return;

		auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		int64_t epochTimeMillisecond = ms.time_since_epoch().count();

		static int64_t nextHeartbeat = 0;
		if (nextHeartbeat <= epochTimeMillisecond) {	//note:this sends the heartbeat early
			if (nextHeartbeat <= 0) nextHeartbeat = epochTimeMillisecond;
			nextHeartbeat += heartbeatInterval;

			std::string str = std::to_string(lastSReceived);
			send("{\"op\":1,\"d\":" + str + "}");
			onHeartbeat();
		}
	}
}