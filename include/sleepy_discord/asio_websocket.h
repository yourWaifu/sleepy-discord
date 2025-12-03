#pragma once
#include "client.h"
#include "message_receiver.h"
#include "asio_schedule.h"
#include "asio_websocketconnection.h"

namespace SleepyDiscord {

	class ASIOWebSocketDiscordClient : public BaseDiscordClient {
	public:
		ASIOWebSocketDiscordClient() = default;
		ASIOWebSocketDiscordClient(const std::string token, const char _n = 0) 
			: ASIOWebSocketDiscordClient(std::make_shared<asio::io_context>(), token) {}
		ASIOWebSocketDiscordClient(std::shared_ptr<asio::io_context> _ioContext, const std::string token) :
			ioContext(_ioContext)
		{
			setup(_ioContext, token);
		}
		~ASIOWebSocketDiscordClient() {}

		std::shared_ptr<asio::io_context> ioContext;
		void setup(std::shared_ptr<asio::io_context> _ioContext, const std::string token) {
			if (workGuard && workGuard->owns_work()) {
				onError(GENERAL_ERROR, "can't set io context while client has work");
				return;
			}

			ioContext = _ioContext;
			setScheduleHandler<ASIOScheduleHandler>(ioContext);

			start(token);
		}

		void run() override {
			if (!ioContext) {  // shouldn't happen but can if default constructor is used without setup
				onError(SleepyDiscord::GENERAL_ERROR, "Can't run Discord Client without async IO or Discord token.");
				return;
			}

			BaseDiscordClient::connect();
			workGuard = std::make_shared<asio::executor_work_guard<asio::io_context::executor_type>>(asio::make_work_guard(*ioContext));
			ioContext->run();
		}

		Timer schedule(TimedTask code, const time_t milliseconds) override {
			return ASIOScheduleHandler::schedule(*ioContext, std::move(code), milliseconds);
		}

		void postTask(PostableTask code) override {
			asio::post(ioContext->get_executor(), code);
		}
	protected:
#include "standard_config_header.h"
	private:
		
		std::shared_ptr<asio::executor_work_guard<asio::io_context::executor_type>> workGuard;

		bool connect(const std::string& uri,
			GenericMessageReceiver& messageProcessor,
			WebsocketConnection& connection
		) override {
			// set up websocket events
			// The connection object will own the eventListener
			// we can capture messageProcessor because the messageProcessor outlives the connection object
			ASIOWebSocketConnection::EventReceiver eventListener;
			eventListener.setOnMessage([&messageProcessor](std::vector<uint8_t> data) { // copy stream to vector
				auto message = std::make_shared<std::string>(data.begin(), data.end()); // copy again to string, dumb but it's simple and memory safe. I'll fix it later, just a temp solution
				messageProcessor.processMessage(WebSocketMessage{ WebSocketMessage::text, *message, message });
			});
			eventListener.setOnClose([&messageProcessor](uint16_t code, std::string _) {
				messageProcessor.processCloseCode(static_cast<WebSocketMessage::OPCode>(code));
			});
			eventListener.setOnOpen([&messageProcessor]() {
				messageProcessor.initialize();
			});

			auto sharedConnection = std::make_shared<ASIOWebSocketConnection>(std::move(eventListener));
			connection = sharedConnection;
			sharedConnection->connect(uri, ioContext);
			return true;
		}

		void disconnect(unsigned int code, const std::string reason, WebsocketConnection& _connection) override {
			if (auto websocketConnection = _connection.lock()) {
				bool startedDisconnect = websocketConnection->disconnect(code, reason);
				if (startedDisconnect) {
					std::cout << "disconnecting\n";
				}
				else {
					// likly already disconnecting
				}
			}
		}

		void send(std::string message, WebsocketConnection& _connection) override {
			if (auto websocketConnection = _connection.lock()) {
				websocketConnection->send(message, WebSocketMessage::text, nullptr);
			}
		}

		void stopClient() override {
			workGuard.reset();
			ioContext->stop();
		}
	};

	typedef ASIOWebSocketDiscordClient DiscordClient;
}