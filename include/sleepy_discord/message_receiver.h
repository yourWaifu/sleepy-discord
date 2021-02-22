#pragma once
#include <string>
#include "websocket_connection.h"
#include "timer.h"

namespace SleepyDiscord {
	struct WebSocketMessage {
		enum OPCode {
			continuation = 0x0,
			text = 0x1,
			binary = 0x2,
			close = 0x8,
			ping = 0x9,
			pong = 0xA,
		};
		using OPCodeType = OPCode;
		OPCodeType opCode = text;
		const std::string& payload;
		//since we are using a reference, we need to keep the payload in memory
		std::shared_ptr<void> lifetime; //not might to be read from
	};

	class GenericMessageReceiver {
	public:
		virtual ~GenericMessageReceiver() = default;
		virtual void initialize() {}                                  //called when ready to recevie messages
		virtual void handleFailToConnect() {}                         //called when connection has failed to start
		virtual void processMessage(const std::string & message) = 0; //called when recevicing a message
		virtual void processCloseCode(const int16_t /*code*/) {}
		virtual void processMessage(const WebSocketMessage message) {
			processMessage(message.payload);
		}
		WebsocketConnection connection;                               //maybe this should be set to protected?
	protected:
		int consecutiveReconnectsCount = 0;
		Timer reconnectTimer;

		inline const time_t getRetryDelay() {
			return consecutiveReconnectsCount < 50 ? consecutiveReconnectsCount * 5000 : 5000 * 50;
		}
	};
}