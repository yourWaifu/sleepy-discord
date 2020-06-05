#include "websocketpp_websocket.h"
#ifndef NONEXISTENT_WEBSOCKETPP
#include <future>

namespace SleepyDiscord {
	void handleTimer(const websocketpp::lib::error_code &ec, std::function<void()>& code) {
		if (ec != websocketpp::transport::error::operation_aborted) {
			code();
		}
	}

	Timer WebsocketppScheduleHandler::schedule(TimedTask code, const time_t milliseconds) {
		auto timer = client.set_timer(
			static_cast<long>(milliseconds),
			websocketpp::lib::bind(&handleTimer, websocketpp::lib::placeholders::_1, code)
		);
		return Timer([timer]() {
			timer->cancel();
		});
	}

	WebsocketppDiscordClient::WebsocketppDiscordClient(const std::string token, const char numOfThreads) :
		_thread(nullptr)
	{
		init();
		setScheduleHandler<WebsocketppScheduleHandler>(this_client);
		start(token, numOfThreads);
	}

	WebsocketppDiscordClient::~WebsocketppDiscordClient() {
		if (_thread != nullptr && _thread->joinable()) _thread->join();
		else _thread.reset();
	}

	void WebsocketppDiscordClient::init() {
		// set up access channels to only log interesting things
		this_client.clear_access_channels(websocketpp::log::alevel::all);
		this_client.set_access_channels(websocketpp::log::alevel::connect);
		this_client.set_access_channels(websocketpp::log::alevel::disconnect);
		this_client.set_access_channels(websocketpp::log::alevel::app);

		this_client.set_tls_init_handler([](websocketpp::connection_hdl) {
			return websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::tlsv1);
		});

		// Initialize the Asio transport policy
		this_client.init_asio();
		this_client.start_perpetual();
	}

	bool WebsocketppDiscordClient::connect(const std::string & uri,
		GenericMessageReceiver* messageProcessor,
		WebsocketConnection& _connection
	) {
		// Create a new connection to the given URI
		websocketpp::lib::error_code ec;
		// Note: there's might be a memory leak caused by get_connection
		_client::connection_ptr con = this_client.get_connection(uri, ec);

		if (ec) {
			onError(GENERAL_ERROR, "Connect initialization: " + ec.message());
			return false;
		}

		con->set_open_handler(std::bind(&WebsocketppDiscordClient::onOpen, this,
			websocketpp::lib::placeholders::_1, messageProcessor
		));

		con->set_close_handler(std::bind(&WebsocketppDiscordClient::onClose, this,
			websocketpp::lib::placeholders::_1, messageProcessor
		));

		con->set_message_handler(std::bind(&WebsocketppDiscordClient::onMessage, this,
			websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2,
			messageProcessor
		));

		con->set_fail_handler(std::bind(&WebsocketppDiscordClient::onFail, this,
			websocketpp::lib::placeholders::_1, messageProcessor
		));

#ifdef SLEEPY_WEBSCOKETPP_PRINTALL
		this_client.set_access_channels(websocketpp::log::alevel::all);
		this_client.set_error_channels(websocketpp::log::alevel::all);
#endif

		if (_connection.expired() == false)
			_connection.reset();

		_connection = con->get_handle();
		// Queue the connection. No DNS queries or network connections will be
		// made until the io_service event loop is run.
		this_client.connect(con);
		return true;
	}

	void WebsocketppDiscordClient::run() {
		this_client.run();
	}

	void handleTimers(const websocketpp::lib::error_code &ec, std::function<void()>& code, _client::timer_ptr timer) {
		if (ec != websocketpp::transport::error::operation_aborted) {
			code();
		}
	}

	template<class Client>
	void rerunTimer(_client::timer_ptr timer, TimedTask code, Client& c) {
		timer->async_wait(
			std::bind(
				&_client::type::handle_timer,
				c.this_client,
				timer,
				std::bind(
					&handleTimers, websocketpp::lib::placeholders::_1, code, timer),
				websocketpp::lib::placeholders::_1
			)
		);
	}

	Timer WebsocketppDiscordClient::schedule(TimedTask code, const time_t milliseconds) {
		_client::timer_ptr timer;
		auto callback = std::bind(
			&handleTimers, websocketpp::lib::placeholders::_1, code, timer);
		timer = this_client.set_timer(
			static_cast<long>(milliseconds),
			callback
		);

		return Timer(
			[timer]() {
				timer->cancel();
			}
		);
	}

	void WebsocketppDiscordClient::runAsync() {
		if (!_thread) _thread.reset(new websocketpp::lib::thread(&WebsocketppDiscordClient::run, this));
	}

	void WebsocketppDiscordClient::onFail(websocketpp::connection_hdl _handle, GenericMessageReceiver* messageProcessor) {
		messageProcessor->handleFailToConnect();
	}

	void WebsocketppDiscordClient::send(std::string message, WebsocketConnection& _connection) {
		websocketpp::lib::error_code error;
		this_client.send(_connection, message, websocketpp::frame::opcode::text, error);
		//temp solution: ingnore all errors
		//Besides the library can detect bad connections by itself anyway
	}

	void WebsocketppDiscordClient::onOpen(websocketpp::connection_hdl hdl,
		GenericMessageReceiver* messageProcessor) {
		initialize(messageProcessor);
	}

	void WebsocketppDiscordClient::onMessage(
		websocketpp::connection_hdl hdl,
		websocketpp::config::asio_client::message_type::ptr msg,
		GenericMessageReceiver* messageProcessor) {
		postTask([=]() { messageProcessor->processMessage(msg->get_payload()); });
		//messageProcessor->processMessage(msg->get_payload());
	}

	//UDPClient WebsocketppDiscordClient::createUDPClient() {
	//	return UDPClient(this_client.get_io_service());
	//}
	
	void WebsocketppDiscordClient::disconnect(
		unsigned int code,
		const std::string reason,
		WebsocketConnection& _connection
	) {
		if (!_connection.expired()) {
			websocketpp::lib::error_code error;
			this_client.close(_connection, static_cast<websocketpp::close::status::value>(code), reason, error);
			//temp fix ignore errors
		}
		std::cout << "disconnect\n";
	}

	void WebsocketppDiscordClient::onClose(websocketpp::connection_hdl _handle,
		GenericMessageReceiver* messageProcessor) {
		_client::connection_ptr con = this_client.get_con_from_hdl(_handle);
		const int16_t closeCode = con->get_remote_close_code();
		std::cout << "Close " << closeCode << ' ' << con->get_remote_close_reason() << '\n';
		messageProcessor->processCloseCode(closeCode);
	}

#include "standard_config.h"
}
#endif