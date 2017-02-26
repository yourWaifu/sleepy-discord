#include "client.h"
#include "experimental.h"

namespace SleepyDiscord {
	DiscordClient::DiscordClient(const std::string _token) 
		: client(&token, this) 
		, clock_thread(&DiscordClient::runClock_thread, this)
	{
		token = _token;
		auto a = cpr::Post(cpr::Url{ "https://discordapp.com/api/gateway" });
		char theGateway[64];
		for (unsigned int position = 0, j = 0; ; ++position) {
			if (a.text[position] == '"')
				++j;
			else if (j == 3) {
				const unsigned int start = position;
				while (a.text[++position] != '"');
				const unsigned int size = position - start;
				a.text.copy(theGateway, size, start);
				theGateway[size] = 0;
				break;
			} 
		}
		client.connect(theGateway/* + "/?v=6"*/);	//TO-DO add v=6 support
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
			{ "Authorization", "Bot " + token },
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
			const char* names[] = { "code", "message" };	//parse json to get code and message
			constexpr unsigned int arraySize = sizeof(names) / sizeof(*names);
			std::string values[arraySize];
			json::getValues(response.text.c_str(), names, values, arraySize);	//parse
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

	inline bool SleepyDiscord::DiscordClient::isMagicReal() {
		return (magic[0] == 'm' && magic[1] == 'A' && magic[2] == 'g' && magic[3] == 'i' && magic[4] == 'c' && magic[5] == 0);
	}

	Message DiscordClient::sendMessage(std::string channel_id, std::string message, bool tts) {
		auto r = request(Post, "channels/" + channel_id + "/messages", "{\"content\":\"" + message + (tts?"\",\"tts\":\"true\"":"\"") +"}");
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

	bool DiscordClient::deleteMessage(const std::string channel_id, const std::string * message_id, const int numOfMessages) {
		return request(Delete, "channels/" + channel_id + "/messages/" + *message_id).status_code == NO_CONTENT;
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

	void DiscordClient::waitTilReady() {
		while (!ready) std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	DiscordClient::WebsocketClient::WebsocketClient(std::string* _token, DiscordClient* _discordClient) {
		token = _token;
		discord = _discordClient;
		init();
	}

	void DiscordClient::runClock_thread() {
		ready = false;
		waitTilReady();
		int HalfSecondTimer = 0;
		while (ready) {
			client.heartbeat();
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

	DiscordClient::WebsocketClient::~WebsocketClient() {
	}

	void DiscordClient::WebsocketClient::init() {
		// set up access channels to only log interesting things
		this_client.clear_access_channels(websocketpp::log::alevel::all);
		this_client.set_access_channels(websocketpp::log::alevel::connect);
		this_client.set_access_channels(websocketpp::log::alevel::disconnect);
		this_client.set_access_channels(websocketpp::log::alevel::app);

		this_client.set_tls_init_handler([this](websocketpp::connection_hdl) {
			return websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::tlsv1);
		});

		// Initialize the Asio transport policy
		this_client.init_asio();

		this_client.set_message_handler(std::bind(&WebsocketClient::onMessage, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		this_client.set_open_handler(std::bind(&WebsocketClient::onOpen, this, websocketpp::lib::placeholders::_1));

	}

	int DiscordClient::WebsocketClient::connect(const std::string & uri) {
		// Create a new connection to the given URI
		websocketpp::lib::error_code ec;
		_client::connection_ptr con = this_client.get_connection(uri, ec);

		if (ec) {
			std::cout << "> Connect initialization error: " << ec.message() << std::endl;
			return -1;
		}
		// Grab a handle for this connection so we can talk to it in a thread 
		// safe manor after the event loop starts.
		handle = con->get_handle();
		// Queue the connection. No DNS queries or network connections will be
		// made until the io_service event loop is run.
		this_client.connect(con);
		_thread.reset(new websocketpp::lib::thread(&_client::run, &this_client));
		return 0;
	}

	void DiscordClient::WebsocketClient::onMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg) {
		const char* names[] = { "op", "d", "s", "t" };
		const unsigned int arraySize = sizeof(names) / sizeof(*names);
		std::string values[arraySize];
		json::getValues(msg->get_payload().c_str(), names, values, arraySize);
		int op = std::stoi(values[0]);
		if (op == 0) {
			lastSReceived = std::stoi(values[2]);
			const std::string &t = values[3];
			std::string *d = values + 1;
			if (t == "READY") {
				heartbeatInterval = std::stoi(json::getValue(d->c_str(), "heartbeat_interval"));
				discord->onReady(d);
				discord->ready = true;
			} else if (t == "MESSAGE_CREATE") {
				discord->onMessage(d);
			} else if (t == "MESSAGE_UPDATE") {
				discord->onEditedMessage(d);
			} else if (t == "GUILD_CREATE") {
				discord->onServer(d);
			} else if (t == "CHANNEL_CREATE") {
				discord->onChannel(d);
			} else if (t == "GUILD_ROLE_CREATE") {
				discord->onEditedRole(d);
			}
		}
	}

	void DiscordClient::WebsocketClient::heartbeat(int op_code) {
		if (!heartbeatInterval) return;

		auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		int64_t epochTimeMillisecond = ms.time_since_epoch().count();

		static int64_t nextHeartbeat = 0;
		if (nextHeartbeat <= epochTimeMillisecond) {	//note:this sends the heartbeat early
			if (nextHeartbeat <= 0) nextHeartbeat = epochTimeMillisecond;
			nextHeartbeat += heartbeatInterval;

			std::string str = std::to_string(lastSReceived);
			this_client.send(handle, "{\"op\":1,\"d\":" + str + "}", websocketpp::frame::opcode::text);
			discord->onHeartbeat();
		}
	}

	void DiscordClient::WebsocketClient::onOpen(websocketpp::connection_hdl hdl) {
		//Handshaking
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
		this_client.send(handle, "{\"op\":2,\"d\":{\"token\":\"" + *token + "\",\"properties\":{\"$os\":\"windows 10\",\"$browser\":\"Sleepy_Discord\",\"$device\":\"Sleepy_Discord\",\"$referrer\":\"\",\"$referring_domain\":\"\"},\"compress\":false,\"large_threshold\":250}}", websocketpp::frame::opcode::text);
	}

}