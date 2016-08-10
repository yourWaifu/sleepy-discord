#include "client.h"

namespace SleepyDiscord {
	DiscordClient::DiscordClient(const std::string _token) 
		: client(&token, this) 
		, clock_thread(&DiscordClient::runClock_thread, this)
	{
		ready = false;
		token = _token;
		auto a = cpr::Post(cpr::Url{ "https://discordapp.com/api/gateway" });
		char theGateway[64];
		for (int i = 0, j = 0, w = 0, m = a.text.length(); i < m; i++) {
			if (a.text[i] == '\"') ++j;
			if (j == 3 && a.text[i] != '\"') theGateway[w++] = a.text[i];
			if (j == 4) theGateway[w] = 0;
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
			contentType });
		updateRateLimiter();
		switch (method) {
		case Post: return session.Post(); break;
		case Patch: return session.Patch(); break;
		case Delete: return session.Delete(); break;
		default: {		//unexpected method
			cpr::Response r;
			r.status_code = 400;
			return r;
		} break;
		}
	}

	cpr::Response DiscordClient::request(RequestMethod method, std::string url, cpr::Multipart multipartParameters) {
		return request(method, url, "", cpr::Parameters{}, multipartParameters);
	}

	cpr::Response DiscordClient::request(RequestMethod method, std::string url, cpr::Parameters httpParameters) {
		return request(method, url, "", httpParameters);
	}

	int DiscordClient::sendMessage(std::string channel_id, std::string message) {
		if (MAX_MESSAGES_SENT_PER_MINUTE <= numOfMessagesSent) return 429;	//Error Code for too many request
		auto r = request(Post, "channels/" + channel_id + "/messages", "{\"content\": \"" + message + "\"}");
		return r.status_code;
	}

	int DiscordClient::uploadFile(std::string channel_id, std::string fileLocation, std::string message) {
		auto r = request(Post, "channels/" + channel_id + "/messages", 
		                 cpr::Multipart{ { "content", message },
		                                 {"file", cpr::File{fileLocation} }
		                 }
		);
		return r.status_code;
	}

	int DiscordClient::editMessage(std::string channel_id, std::string message_id, std::string newMessage) {
		if (MAX_MESSAGES_SENT_PER_MINUTE <= numOfMessagesSent) return 429;	//Error Code for too many request
		auto r = request(Patch, "channels/" + channel_id + "/messages/" + message_id, "{\"content\": \"" + newMessage + "\"}");
		return r.status_code;
	}

	int DiscordClient::deleteMessage(const std::string channel_id, const std::string * message_id, const int numOfMessages) {
		if (MAX_MESSAGES_SENT_PER_MINUTE <= numOfMessagesSent) return 429;	//Error Code for too many request
		auto r = request(Delete, "channels/" + channel_id + "/messages/" + *message_id);
		return r.status_code;
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
		waitTilReady();
		while (ready) { 
			client.heartbeat();
			boost::this_thread::sleep(boost::posix_time::milliseconds(500));
			if (MAX_MESSAGES_SENT_PER_MINUTE <= ++rateLimiterClock)
				rateLimiterClock = 0;
			numOfMessagesSent -= rateLimiter[rateLimiterClock];
			rateLimiter[rateLimiterClock] = 0;
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
			return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
		});

		// Initialize the Asio transport policy
		this_client.init_asio();

		this_client.set_message_handler(std::bind(&WebsocketClient::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		this_client.set_open_handler(std::bind(&WebsocketClient::on_open, this, websocketpp::lib::placeholders::_1));

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

	void DiscordClient::WebsocketClient::on_message(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg) {
		std::string messagePayload = msg->get_payload();
		JSON* jsonMessage = JSON_parseJSON(messagePayload.c_str(), messagePayload.length());
		int op = (int)*(double*)JSON_access1(jsonMessage, "op");
		if (op == 0) {
			lastSReceived = (int)*(double*)JSON_access1(jsonMessage, "s");
			std::string t = (char*)JSON_access1(jsonMessage, "t");
			if (t == "READY") {
				heartbeatInterval = (int)*(double*)JSON_access(jsonMessage, 2, "d", "heartbeat_interval");
				discord->onReady(jsonMessage);
				discord->ready = true;
			} else if (t == "MESSAGE_CREATE") {
				discord->onMessage(jsonMessage);
			} else if (t == "MESSAGE_UPDATE") {
				discord->onEditedMessage(jsonMessage);
			} else if (t == "GUILD_CREATE") {
				discord->onGiuld(jsonMessage);
			}
		}
		JSON_deallocate(jsonMessage);
	}

	void DiscordClient::WebsocketClient::heartbeat(int op_code) {
		if (!heartbeatInterval) return;

		boost::posix_time::ptime const time_epoch(boost::gregorian::date(1970, 1, 1));
		auto ms = (boost::posix_time::microsec_clock::local_time() - time_epoch).total_microseconds();
		double epochTimeMillisecond = ms * .001;

		static double nextHeartbeat = 0;
		if (nextHeartbeat <= epochTimeMillisecond) {
			if (!nextHeartbeat) nextHeartbeat = epochTimeMillisecond;
			nextHeartbeat += heartbeatInterval;

			std::string str = boost::lexical_cast<std::string>(lastSReceived);
			this_client.send(handle, "{\"op\":1,\"d\":" + str + "}", websocketpp::frame::opcode::text);
			discord->onHeartbeat();
		}
	}

	void DiscordClient::WebsocketClient::on_open(websocketpp::connection_hdl hdl) {
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