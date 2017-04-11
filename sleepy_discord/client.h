#pragma once
#include <string>
//#include <boost/thread.hpp>
#include <thread>
#include <cpr/cpr.h>
#include <chrono>
#include "json.h"

//objects
#include "message.h"
#include "channel.h"
#include "server.h"
#include "invite.h"
#include "webhook.h"

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

namespace SleepyDiscord {
#define TOKEN_SIZE 64
#define MAX_MESSAGES_SENT_PER_MINUTE 120	//you should replace remove those
#define MILLISECONDS_PER_MESSAGES_SENT 60000/MAX_MESSAGES_SENT_PER_MINUTE
	class BaseDiscordClient {
	public:
		BaseDiscordClient() {}
		BaseDiscordClient(const std::string _token) { start(_token); }
		~BaseDiscordClient();
		
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

		const std::string path(const char* source, ...);	//only works with std::strings

		void testFunction(std::string teststring);

		//channel functions
		Message sendMessage(std::string channel_id, std::string message, bool tts = false);
		Message uploadFile(std::string channel_id, std::string fileLocation, std::string message);
		Message editMessage(std::string channel_id, std::string message_id, std::string newMessage);
		bool deleteMessage(const std::string channel_id, const std::string* message_id, const unsigned int numOfMessages = 1);
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

		//Invite functions
		Invite inviteEndpoint(RequestMethod method, std::string inviteCode);
		Invite getInvite(std::string inviteCode);                                                                             //to do test this
		Invite deleteInvite(std::string inviteCode);                                                                          //to do test this
		Invite acceptInvite(std::string inviteCode);	//not available to bot accounts                                       //to do test this

		//User functions
		User getCurrentUser();                                                                                                //to do test this
		User getUser(std::string user_id);                                                                                    //to do test this
		//User editCurrentUser();
		//getCurrentUserServers			//I think you could rename this getCurrentServers
		bool leaveServer(std::string server_id);                                                                              //to do test this
		//getDirectMessageChannels
		//createDirectMessageChannel
		//createGroupDirectMessageChannel
		//getUserConnections

		//Voice Functions
		//getVoiceRegions

		//Webhook functions
		Webhook createWebhook(std::string channel_id, std::string name, std::string avatar = "");
		//Webhook getChannelWebhooks();
		//Webhook getServerWebhooks();
		Webhook getWebhook(std::string webhook_id, std::string webhookToken = "");
		Webhook editWebhook(std::string webhook_id, std::string webhookToken, std::string name, std::string avatar);    //you can leave token or avatar as null
		bool deleteWebhook(std::string webhook_id, std::string webhookToken = "");
		//Webhook excuteWebhook(std::string webhook_id, std::string webhookToken, bool wait = false)

		//websocket functions
		void updateStatus(time_t idleSince, std::string gameName);

		void waitTilReady();
		const bool isReady() { return ready; }
		void quit();	//public function for diconnecting
	protected:
		virtual void onReady(std::string* jsonMessage);
		virtual void onMessage(std::string* jsonMessage);
		virtual void onEditedMessage(std::string* jsonMessage);
		virtual void onHeartbeat();
		virtual void onHeartbeatAck();
		virtual void onServer(std::string* jsonMessage);
		virtual void onChannel(std::string* jsonMessage);
		virtual void onEditedRole(std::string* jsonMessage);
		virtual void onInvaldSession();
		virtual void onDisconnet();
		virtual void onQuit();
		virtual void tick(float deltaTime);
		virtual void onError(ErrorCode errorCode, const std::string errorMessage);

		/*do not use or overwrite the protected values below,
		unless you know what you are doing*/
		void processMessage(std::string message);
		/*0 is for the timer thread
		1 is for the op 10 packet
		2 is for the ready packet or everything else*/
		void heartbeat(int op_code = 0);
		inline std::string getToken() { return *token.get(); }
		void start(const std::string _token, const char maxNumOfThreads = 2);
		virtual bool connect(const std::string & uri) { return false; }
		virtual void send(std::string message) {}
		virtual void disconnect(unsigned int code, const std::string reason) {}
	private:
		bool isHeartbeatRunning;
		int heartbeatInterval = 0;
		int lastSReceived;
		bool wasHeartbeatAcked = true;

		enum OPCode {
			DISPATCH              = 0,		//dispatches an event
			HEARTHBEAT            = 1,		//used for ping checking
			IDENTIFY              = 2,		//used for client handshake
			STATUS_UPDATE         = 3,		//used to update the client status
			VOICE_STATE_UPDATE    = 4,	 	//used to join / move / leave voice channels
			VOICE_SERVER_PING     = 5,	 	//used for voice ping checking
			RESUME                = 6,		//used to resume a closed connection
			RECONNECT             = 7,		//used to tell clients to reconnect to the gateway
			REQUEST_GUILD_MEMBERS = 8,		//used to request guild members
			INVALID_SESSION       = 9,		//used to notify client they have an invalid session id
			HELLO                 = 10,		//sent immediately after connecting, contains heartbeat and server debug information
			HEARTBEAT_ACK         = 11,		//sent immediately following a client heartbeat that was received
		};

		std::thread clock_thread;
		void runClock_thread();

		void updateRateLimiter(const uint8_t numOfMessages = 1);

		std::unique_ptr<std::string> token;		//stored in a unique_ptr so that you can't see it in the debugger
		std::string session_id;
		void getTheGateway();
		char theGateway[32];
		bool ready;
		void sendIdentity();
		bool restart();		//it's like start but when it already started
		void reconnect(const unsigned int status = 1000);
		void disconnectWebsocket(unsigned int code, const std::string reason = "");

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
