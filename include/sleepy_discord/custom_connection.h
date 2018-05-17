#pragma once
#include <memory>
#include "websocket.h"

namespace SleepyDiscord {
	struct CustomWebsocketConnection : public GenericWebsocketConnection {
		CustomWebsocketConnection() {}

		CustomWebsocketConnection(GenericWebsocketConnection* pointer) :
			connection(pointer) {}

		template<typename CustomWebsocketConnection, typename... Args>
		void forward(Args&&... args) {
			connection = std::unique_ptr<GenericWebsocketConnection>(
				new CustomWebsocketConnection(std::forward<Args>(args)...)
			);
		}
		
		template<class RealWebsocketConnection>
		inline operator RealWebsocketConnection&() {
			return *static_cast<RealWebsocketConnection>(connection.get());
		}

		inline GenericWebsocketConnection& operator*() const {
			return *connection;
		}

		inline GenericWebsocketConnection* operator->() const {
			return connection.get();
		}

		template<class RealWebsocketConnection>
		inline RealWebsocketConnection& get() const {
			return *static_cast<RealWebsocketConnection*>(connection.get());
		}

	private:
		std::unique_ptr<GenericWebsocketConnection> connection;
	};

	typedef CustomWebsocketConnection WebsocketConnection;
}