#include "websocketpp_websocket.h"

namespace SleepyDiscord {

	WebsocketppDiscordClient::WebsocketppDiscordClient(const std::string token) {
		init();
		start(token);
	}

	WebsocketppDiscordClient::~WebsocketppDiscordClient() {

	}

	void WebsocketppDiscordClient::init() {
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

		this_client.set_message_handler(std::bind(&WebsocketppDiscordClient::onMessage, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		this_client.set_open_handler(std::bind(&WebsocketppDiscordClient::onOpen, this, websocketpp::lib::placeholders::_1));
	}

	int WebsocketppDiscordClient::connect(const std::string & uri) {
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

	void WebsocketppDiscordClient::send(std::string message) {
		this_client.send(handle, message, websocketpp::frame::opcode::text);
	}

	void WebsocketppDiscordClient::onMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg) {
		processMessage(msg->get_payload());
	}

	void WebsocketppDiscordClient::onOpen(websocketpp::connection_hdl hdl) {
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
		send("{\"op\":2,\"d\":{\"token\":\"" + getToken() + "\",\"properties\":{\"$os\":\"windows 10\",\"$browser\":\"Sleepy_Discord\",\"$device\":\"Sleepy_Discord\",\"$referrer\":\"\",\"$referring_domain\":\"\"},\"compress\":false,\"large_threshold\":250}}");
	}
}