#include "websocketpp_websocket.h"
#ifndef NONEXISTENT_WEBSOCKETPP

namespace SleepyDiscord {

	WebsocketppDiscordClient::WebsocketppDiscordClient(const std::string token, const char numOfThreads) :
		maxNumOfThreads(numOfThreads)
	{
		init();
		start(token, numOfThreads);
	}

	WebsocketppDiscordClient::~WebsocketppDiscordClient() {
		if (_thread) _thread->join();
		else _thread.reset();
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
	}

	bool WebsocketppDiscordClient::connect(const std::string & uri,
		GenericMessageReceiver* messageProcessor,
		WebsocketConnection* connection
	) {
		// Create a new connection to the given URI
		websocketpp::lib::error_code ec;
		_client::connection_ptr con = this_client.get_connection(uri, ec);

		if (ec) {
			onError(GENERAL_ERROR, "Connect initialization: " + ec.message());
			return false;
		}
		
		con->set_open_handler(std::bind(&WebsocketppDiscordClient::onOpen, this,
			websocketpp::lib::placeholders::_1, messageProcessor
		));

		con->set_message_handler(std::bind(&WebsocketppDiscordClient::onMessage, this,
			websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2,
			messageProcessor
		));

		*connection = con->get_handle();
		// Queue the connection. No DNS queries or network connections will be
		// made until the io_service event loop is run.
		this_client.connect(con);
		return true;
	}

	void WebsocketppDiscordClient::run() {
		this_client.run();
	}

	void handleTimers(const websocketpp::lib::error_code &ec, std::function<void()>& code) {
		if (ec == websocketpp::transport::error::operation_aborted) return;
		else code();
	}

	Timer WebsocketppDiscordClient::schedule(std::function<void()> code, const time_t milliseconds) {
		auto timer = this_client.set_timer(
			milliseconds,
			websocketpp::lib::bind(&handleTimers, websocketpp::lib::placeholders::_1, code)
		);
		return Timer([timer]() {
			timer->cancel();
		});
	}

	void WebsocketppDiscordClient::runAsync() {
		if (!_thread) _thread.reset(new websocketpp::lib::thread(&_client::run, &this_client));
	}

	void WebsocketppDiscordClient::send(std::string message, WebsocketConnection* connection) {
		websocketpp::lib::error_code error;
		this_client.send(*connection, message, websocketpp::frame::opcode::text, error);
		//temp solution
		//Besides the library can detect bad connections by itself anyway
		if(error && error != websocketpp::error::bad_connection)
			throw websocketpp::exception(error);
	}

	void WebsocketppDiscordClient::onOpen(websocketpp::connection_hdl hdl,
		GenericMessageReceiver * messageProcessor) {
		initialize(messageProcessor);
	}

	void WebsocketppDiscordClient::onMessage(
		websocketpp::connection_hdl hdl,
		websocketpp::config::asio_client::message_type::ptr msg,
		GenericMessageReceiver* messageProcessor) {
		processMessage(messageProcessor, msg->get_payload());
	}
	
	void WebsocketppDiscordClient::disconnect(
		unsigned int code,
		const std::string reason,
		WebsocketConnection* connection) {
		if (!connection->expired())
			this_client.close(*connection, code, reason);
	}

	void WebsocketppDiscordClient::onClose(_client * client, websocketpp::connection_hdl handle) {

	}

#include "standard_config.h"
}
#endif