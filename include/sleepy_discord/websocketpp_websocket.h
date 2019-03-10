#pragma once
#include "websocketpp_common.h"
#ifndef NONEXISTENT_WEBSOCKETPP
#include <chrono>
//#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include "client.h"
#include "websocketpp_connection.h"
#include "asio_schedule.h"
#include "asio_udp.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> _client;

namespace SleepyDiscord {
	//typedef GenericMessageReceiver MessageProcssor;

	class WebsocketppScheduleHandler : public ASIOBasedScheduleHandler {
	public:
		WebsocketppScheduleHandler(_client& c) : client(c) {}
		~WebsocketppScheduleHandler() = default;
		Timer schedule(TimedTask code, const time_t milliseconds) override;
		inline websocketpp::lib::asio::io_service& getIOService() override {
			return client.get_io_service();
		}
	private:
		_client& client;
	};

	class WebsocketppDiscordClient : public BaseDiscordClient {
	public:
		WebsocketppDiscordClient() : maxNumOfThreads(0) {}
		WebsocketppDiscordClient(const std::string token, const char numOfThreads = SleepyDiscord::DEFAULT_THREADS);
		~WebsocketppDiscordClient();

		using TimerPointer = std::weak_ptr<websocketpp::lib::asio::steady_timer>;

		void run();
		Timer schedule(TimedTask code, const time_t milliseconds);
		void postTask(PostableTask code) override {
			asio::post(code);
		}
		//UDPClient createUDPClient() /* override*/;
	protected:
#include "standard_config_header.h"
	private:
		void init();
		bool connect(const std::string & uri,
			GenericMessageReceiver* messageProcessor,
			WebsocketConnection& connection
		) override;
		void disconnect(unsigned int code, const std::string reason, WebsocketConnection& connection);
		void onClose(
			websocketpp::connection_hdl handle,
			GenericMessageReceiver* messageProcessor
		);
		void onFail(websocketpp::connection_hdl handle, GenericMessageReceiver* messageProcessor);
		void send(std::string message, WebsocketConnection& connection);
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
	typedef WebsocketppDiscordClient DiscordClient;
}
#else
#ifndef BOOST_VERSION
#undef ASIO_STANDALONE
#undef _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#undef _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#endif
#endif