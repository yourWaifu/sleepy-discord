#pragma once
#include "websocketpp_common.h"
#ifndef NONEXISTENT_WEBSOCKETPP
#include <chrono>
//#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include "client.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> _client;

namespace SleepyDiscord {
	typedef websocketpp::connection_hdl WebsocketConnection;

	//typedef GenericMessageReceiver MessageProcssor;

	class WebsocketppWebsocketClient : public BaseDiscordClient {
	public:
		WebsocketppWebsocketClient() : maxNumOfThreads(0) {}
		WebsocketppWebsocketClient(const std::string token, const char numOfThreads = 3);
		~WebsocketppWebsocketClient();

		void run();
		Timer schedule(std::function<void()> code, const time_t milliseconds);
	protected:
#include "standard_config_header.h"
	private:
		void init();
		bool connect(const std::string & uri,
			GenericMessageReceiver* messageProcessor,
			WebsocketConnection* connection
		);
		void disconnect(unsigned int code, const std::string reason, WebsocketConnection* connection);
		void onClose(_client * client, websocketpp::connection_hdl handle);
		void send(std::string message, WebsocketConnection* connection);
		void runAsync();
		void onOpen(websocketpp::connection_hdl hdl, GenericMessageReceiver* messageProcessor);
		void onMessage(
			websocketpp::connection_hdl hdl,
			websocketpp::config::asio_client::message_type::ptr msg, 
			GenericMessageReceiver* messageProcessor
		);
		_client this_client;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> _thread;
		websocketpp::connection_hdl handle;
		const char maxNumOfThreads;
	};
	typedef WebsocketppWebsocketClient DiscordClient;
}
#else
#ifndef BOOST_VERSION
#undef ASIO_STANDALONE
#undef _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#undef _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#endif
#endif