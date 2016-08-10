#pragma once
#include <websocketpp/config/asio_client.hpp>
//#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <cpr\cpr.h>
#include "json.h"

/*

add better support for rate limits for delete message and others

*/


class WebsocketClient;

typedef websocketpp::client<websocketpp::config::asio_tls_client> _client;

namespace SleepyDiscord {
#define TOKEN_SIZE 64
#define MAX_MESSAGES_SENT_PER_MINUTE 120
#define MILLISECONDS_PER_MESSAGES_SENT 60000/MAX_MESSAGES_SENT_PER_MINUTE
	class DiscordClient {
	public:
		DiscordClient(const std::string _token);
		~DiscordClient();
		
		//http variables and functions
		enum RequestMethod {
			Post,
			Patch,
			Delete
		};
		cpr::Response request(RequestMethod method, std::string url, std::string jsonParameters = "",
			cpr::Parameters httpParameters = cpr::Parameters{}, cpr::Multipart multipartParameters = cpr::Multipart{});
		cpr::Response request(RequestMethod method, std::string url, cpr::Multipart multipartParameters);
		cpr::Response request(RequestMethod method, std::string url, cpr::Parameters httpParameters);
		int sendMessage(std::string channel_id, std::string message);
		int uploadFile(std::string channel_id, std::string fileLocation, std::string message);
		int editMessage(std::string channel_id, std::string message_id, std::string newMessage);
		int deleteMessage(const std::string channel_id, const std::string* message_id, const int numOfMessages = 1);

		void waitTilReady();
		bool isReady() { return ready; }
	protected:
		virtual void onReady(JSON* jsonMessage);
		virtual void onMessage(JSON* jsonMessage);
		virtual void onEditedMessage(JSON* jsonMessage);
		virtual void onHeartbeat();
		virtual void onGiuld(JSON* jsonMessage);
		virtual void tick(float deltaTime);
		virtual void onError(int errorCode);
	private:
		class WebsocketClient {
		public:
			WebsocketClient(std::string* _token, DiscordClient* _discordEvent);
			~WebsocketClient();
			void init();
			int connect(const std::string & uri);
			/*0 is for the timer thread
			1 is for the op 10 packet
			2 is for the ready packet or everything else*/
			void heartbeat(int op_code = 0);	//interval is the amount of ms that the heartbeat needs to be sent to discord
		private:
			std::string* token;
			DiscordClient* discord;
			bool connected;
			_client this_client;
			websocketpp::lib::shared_ptr<websocketpp::lib::thread> _thread;
			int heartbeatInterval = 0;
			int lastSReceived;
			websocketpp::connection_hdl handle;
			void on_message(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg);
			void on_open(websocketpp::connection_hdl hdl);
		};
		boost::thread clock_thread;
		void runClock_thread();
		void updateRateLimiter(const uint8_t numOfMessages = 1);
		WebsocketClient client;
		std::string token;
		bool ready;
		//every 500 milliseconds we'll add 1 to the rateLimiterClock and it's not less then 120 then we go back to 0
		//after that, we'll do numOfMessagesSent - rateLimiter[rateLimiterClock] and set rateLimiter[rateLimiterClock] to 0
		//every time you send a message, we'll add one to rateLimiter[rateLimiterClock] and numOfMessagesSent
		//when rateLimiterClock % 2 == 0, then we'll check if numOfMessagesSent == sum of rateLimiter. this is for debugging purposes
		//and every time rateLimiterClock is 0 set numOfGameUpdates to 0;
		//also max game updates is 2
		//this way the client will never send more then 120 messages per minute;
		uint8_t rateLimiter[MAX_MESSAGES_SENT_PER_MINUTE];
		uint8_t rateLimiterClock;
		uint8_t numOfMessagesSent;
	};
}
