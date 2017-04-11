#pragma once
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#include <websocketpp/config/asio_client.hpp>
#ifndef NONEXISTENT_WEBSOCKETPP
//#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include "client.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> _client;

namespace SleepyDiscord {
	class WebsocketppDiscordClient : public BaseDiscordClient {
	public:
		WebsocketppDiscordClient() : maxNumOfThreads(0) {}
		WebsocketppDiscordClient(const std::string token, const char numOfThreads = 3);
		~WebsocketppDiscordClient();
	private:
		void init();
		bool connect(const std::string & uri);
		void disconnect(unsigned int code, const std::string reason);
		void onClose(_client * client, websocketpp::connection_hdl handle);
		void send(std::string message);
		_client this_client;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> _thread;
		websocketpp::connection_hdl handle;
		void onMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg);
		const char maxNumOfThreads;
	};
	typedef WebsocketppDiscordClient DiscordClient;
}
#else
#undef ASIO_STANDALONE
#undef _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#undef _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#endif