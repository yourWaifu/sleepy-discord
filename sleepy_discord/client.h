#pragma once
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#include <websocketpp/config/asio_client.hpp>
//#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <string>
//#include <boost/thread.hpp>
#include <thread>
#include <cpr/cpr.h>
#include <chrono>	//for debugging
#include "json.h"

//objects
#include "message.h"
#include "channel.h"
#include "server.h"

#include "error.h"
#include "common.h"

/*

add better support for rate limits for delete message and others
make http functions return things other then status codes
remove the boost library
rewrite discordObject
fix random crashing because of failing to send heartbeat
stop using #define and use static const
make function perimeters const

*/


class WebsocketClient;

typedef websocketpp::client<websocketpp::config::asio_tls_client> _client;

namespace SleepyDiscord {
#define TOKEN_SIZE 64
#define MAX_MESSAGES_SENT_PER_MINUTE 120	//you should replace remove those
#define MILLISECONDS_PER_MESSAGES_SENT 60000/MAX_MESSAGES_SENT_PER_MINUTE
	class DiscordClient {
	public:
		DiscordClient(const std::string _token);
		~DiscordClient();
		
		//http variables and functions
		//request functions
		enum RequestMethod {
			Post,
			Patch,
			Delete,
			Get,
			Put
		};
		cpr::Response request(RequestMethod method, std::string url, std::string jsonParameters = "",
			cpr::Parameters httpParameters = cpr::Parameters{}, cpr::Multipart multipartParameters = cpr::Multipart{});
		cpr::Response request(RequestMethod method, std::string url, cpr::Multipart multipartParameters);
		cpr::Response request(RequestMethod method, std::string url, cpr::Parameters httpParameters);

		void testFunction(std::string teststring);

		//channel functions
		Message sendMessage(std::string channel_id, std::string message, bool tts = false);
		Message uploadFile(std::string channel_id, std::string fileLocation, std::string message);
		Message editMessage(std::string channel_id, std::string message_id, std::string newMessage);
		bool deleteMessage(const std::string channel_id, const std::string* message_id, const int numOfMessages = 1);
		Channel deleteChannel(std::string channel_id);
		Channel getChannel(std::string channel_id);                                                                           //to do test this
		//to do: Get Channel Messages
		Message getMessage(std::string channel_id, std::string message_id);                                                   //to do test this
		bool addReaction(std::string channel_id, std::string message_id, std::string emoji);
		bool removeReaction(std::string channel_id, std::string message_id, std::string emoji, std::string user_id = "@me");  //to do test this
		//reaction array getReactions(std::string channel_id, std::string message_id, std::string emoji);
		//to do add bulk delete messages
		//add Edit Channel Permissions
		//add Get Channel Invites
		//Create Channel Invite
		bool sendTyping(std::string channel_id);                                                                              //to do test this
		//message array getPinnedMessages(std::string channel_id);
		bool pinMessage(std::string channel_id, std::string message_id);
		bool unpinMessage(std::string channel_id, std::string message_id);
		//Group DM Add Recipient
		//Group DM Remove Recipient

		//server functions
		int createRole(std::string server_id);                                                                                //to do test this
		//int editRole(std::string role_id, std::string nam.e = "", int permissions = NULL, int position = -1, int color = , bool hoist =);
		bool deleteRole(std::string server_id, std::string role_id);                                                          //to do test this
		bool muteServerMember(std::string server_id, std::string user_id, bool mute = true);                                  //to do test this
		//Create Server
		Server getServer(std::string server_id);                                                                              //to do test this
		//edit Server
		Server deleteServer(std::string server_id);                                                                           //to do test this
		Channel createTextChannel(std::string server_id, std::string name);	                                                  //to do test this
		Channel editChannel(std::string channel_id, std::string name = "", std::string topic = "");	                          //to do test this
		Channel editChannelName(std::string channel_id, std::string name);                                                    //to do test this
		Channel editChannelTopic(std::string channel_id, std::string topic);                                                  //to do test this
		//get member  make a member class
		//listMembers
		//add member
		//edit member
		bool editNickname(std::string server_id, std::string newNickname);
		bool addRole(std::string server_id, std::string member_id, std::string role_id);
		bool removeRole(std::string server_id, std::string member_id, std::string role_id);   //removes role from member
		bool kickMember(std::string server_id, std::string member_id);
		//get bans  needs member class
		bool banMember(std::string server_id, std::string member_id);                                                         //to do test this later
		bool unbanMember(std::string server_id, std::string member_id);                                                       //to do test this later
		//getRoles  make role array
		//edit role position
		//editing roles
		void pruneMembers(std::string server_id, const unsigned int numOfDays);                                               //to do test
		//get prune count
		//Get Voice Regions  needs voice region class
		//Get Invites  needs invite class
		//Get Integrations  needs whatever a integration class is
		//bool createIntegration(std::string server_id, std::string integration_id, );                                        //finish this later
		bool deleteIntegration(std::string server_id, std::string integration_id);                                            //to do test this
		bool syncIntegration(std::string server_id, std::string integration_id);                                              //to do test this
		//get server embed  needs server embed class
		//edit server embed

		void waitTilReady();
		bool isReady() { return ready; }
	protected:
		virtual void onReady(std::string* jsonMessage);
		virtual void onMessage(std::string* jsonMessage);
		virtual void onEditedMessage(std::string* jsonMessage);
		virtual void onHeartbeat();
		virtual void onServer(std::string* jsonMessage);
		virtual void onChannel(std::string* jsonMessage);
		virtual void onEditedRole(std::string* jsonMessage);
		virtual void onDisconnet();
		virtual void tick(float deltaTime);
		virtual void onError(ErrorCode errorCode, std::string errorMessage);
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
			void onMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg);
			void onOpen(websocketpp::connection_hdl hdl);
		};
		std::thread clock_thread;
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

		//checks to make sure this is valid client
		char magic[6];
		inline bool isMagicReal();

		//error handling
		void setError(int errorCode);
	};
}
