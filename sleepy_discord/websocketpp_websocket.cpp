#include "websocketpp_websocket.h"
#ifndef NONEXISTENT_WEBSOCKETPP

namespace SleepyDiscord {

	WebsocketppDiscordClient::WebsocketppDiscordClient(const std::string token, const char numOfThreads) :
		maxNumOfThreads(numOfThreads), _thread(nullptr)
	{
		init();
		start(token, numOfThreads);
	}

	WebsocketppDiscordClient::~WebsocketppDiscordClient() {
		if (_thread->joinable()) _thread->join();
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

		this_client.set_message_handler(std::bind(&WebsocketppDiscordClient::onMessage, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		
		this_client.set_close_handler(std::bind(&WebsocketppDiscordClient::onClose, this,
			websocketpp::lib::placeholders::_1));
	}

	bool WebsocketppDiscordClient::connect(const std::string & uri) {
		// Create a new connection to the given URI
		websocketpp::lib::error_code ec;
		_client::connection_ptr con = this_client.get_connection(uri, ec);

		if (ec) {
			onError(GENERAL_ERROR, "Connect initialization: " + ec.message());
			return false;
		}
		// Grab a handle for this connection so we can talk to it in a thread 
		// safe manor after the event loop starts.
		handle = con->get_handle();
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

	void WebsocketppDiscordClient::send(std::string message) {
		websocketpp::lib::error_code error;
		this_client.send(handle, message, websocketpp::frame::opcode::text, error);
		//temp solution
		//Besides the library can detect bad connections by itself anyway
		if(error && error != websocketpp::error::bad_connection)
			throw websocketpp::exception(error);
	}

	void WebsocketppDiscordClient::onMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg) {
		processMessage(msg->get_payload());
	}
	
	void WebsocketppDiscordClient::disconnect(unsigned int code, const std::string reason) {
		this_client.close(handle, code, reason);
	}

	void WebsocketppDiscordClient::onClose(websocketpp::connection_hdl handle) {
		_client::connection_ptr con = this_client.get_con_from_hdl(handle);
		const int16_t closeCode = con->get_remote_close_code();
		std::cout << "Close " << closeCode << ' ' << con->get_remote_close_reason() << '\n'; 
		processCloseCode(closeCode);
	}

#include "standard_config.h"
}
#endif
