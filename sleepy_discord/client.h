#pragma once
#include <string>
#include <thread>

//objects
#include "message.h"
#include "channel.h"
#include "server.h"
#include "invite.h"
#include "webhook.h"
#include "permissions.h"

#include "error.h"
#include "common.h"
#include "http.h"

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
#define MAX_MESSAGES_SENT_PER_MINUTE 116
#define HALF_MINUTE_MILLISECONDS 30000
#define MAX_MESSAGES_SENT_PER_HALF_MINUTE MAX_MESSAGES_SENT_PER_MINUTE/2
	class BaseDiscordClient {
	public:
		BaseDiscordClient() {}
		BaseDiscordClient(const std::string _token) { start(_token); }
		~BaseDiscordClient();

		Response request(const RequestMethod method, std::string url, std::string jsonParameters = ""/*,
			cpr::Parameters httpParameters = cpr::Parameters{}, cpr::Multipart multipartParameters = cpr::Multipart{});
		Response request(const RequestMethod method, std::string url, cpr::Multipart multipartParameters);
		Response request(const RequestMethod method, std::string url, cpr::Parameters httpParameters*/);

		const std::string path(const char* source, ...);	//only works with std::strings

		void testFunction(std::string teststring);

		//channel functions
		Channel getChannel(std::string channel_id);                                                                           //to do test this
		Channel deleteChannel(std::string channel_id);
		enum GetMessagesKey { around, before, after };
		std::vector<Message> getMessages(std::string channel_id, GetMessagesKey when, std::string message_id, uint8_t limit);  //to do test this
		Message getMessage(std::string channel_id, std::string message_id);                                                   //to do test this, and add more then one message return
		Message sendMessage(std::string channel_id, std::string message, bool tts = false);
		Message uploadFile(std::string channel_id, std::string fileLocation, std::string message);
		bool addReaction(std::string channel_id, std::string message_id, std::string emoji);
		bool removeReaction(std::string channel_id, std::string message_id, std::string emoji, std::string user_id = "@me");  //to do test this
		std::vector<Reaction> getReactions(std::string channel_id, std::string message_id, std::string emoji);                //to do test this
		void removeAllReactions(std::string channel_id, std::string message_id);                                              //to do test this
		Message editMessage(std::string channel_id, std::string message_id, std::string newMessage);
		bool deleteMessage(const std::string channel_id, const std::string* message_id, const unsigned int numOfMessages = 1);
		/*allow is a bitwise value of all allowed permissions
		deny is a bitwise value of all deisallowed permissions
		type is "member" for a user or "role" for a role*/
		bool editChannelPermissions(std::string channel_id, std::string id, int allow, int deny, std::string type);           //to do test this
		std::vector<Invite> getChannelInvites(std::string channel_id);                                                        //to do test this
		Invite createChannelInvite(std::string channel_id, const uint64_t maxAge = 0, const uint64_t maxUses = 0, const bool temporary = false, const bool unique = false);  //to do test this
		bool removeChannelPermission(std::string channel_id, std::string id);
		bool sendTyping(std::string channel_id);                                                                              //to do test this
		std::vector<Message> getPinnedMessages(std::string channel_id);                                                       //to do test this
		bool pinMessage(std::string channel_id, std::string message_id);
		bool unpinMessage(std::string channel_id, std::string message_id);
		void addRecipient(std::string channel_id, std::string user_id);
		void removeRecipient(std::string channel_id, std::string user_id);
		/*functions with more then one name to make life easy for users that use IntelliSense*/
		inline bool deleteReaction(std::string channel_id, std::string message_id, std::string emoji) { removeReaction(channel_id, message_id, emoji); }
		inline void deleteAllReactions(std::string channel_id, std::string message_id) { removeAllReactions(channel_id, message_id); }
		inline bool deleteChannelPermission(std::string channel_id, std::string id) { removeChannelPermission(channel_id, id); }
		inline void deleteRecipient(std::string channel_id, std::string user_id) { removeRecipient(channel_id, user_id); }

		//server functions
		//Server createServer(std::string name, std::string region, std::string icon, int verificationLevel, int defaultMessageNotifications, std::vector<Role> roles, )
		Server deleteServer(std::string server_id);                                                                           //to do test this
		std::vector<Channel> GetServerChannels(std::string server_id);                                                        //to do test this
		Channel createTextChannel(std::string server_id, std::string name);	                                                  //to do test this
		void getServerMember(std::string server_id, std::string user_id);                                                     //to do test this
		void listServerMembers(std::string server_id, uint16_t limit = 0, std::string after = "");
		Server getServer(std::string server_id);                                                                              //to do test this
		//edit Server
		Channel editChannel(std::string channel_id, std::string name = "", std::string topic = "");	                          //to do test this
		Channel editChannelName(std::string channel_id, std::string name);                                                    //to do test this
		Channel editChannelTopic(std::string channel_id, std::string topic);                                                  //to do test this
		//get member  make a member class
		//listMembers
		//add member
		//edit member
		bool muteServerMember(std::string server_id, std::string user_id, bool mute = true);                                  //to do test this
		bool editNickname(std::string server_id, std::string newNickname);
		bool addRole(std::string server_id, std::string member_id, std::string role_id);
		bool removeRole(std::string server_id, std::string member_id, std::string role_id);   //removes role from member
		bool kickMember(std::string server_id, std::string member_id);
		//get bans  needs member class
		bool banMember(std::string server_id, std::string member_id);                                                         //to do test this later
		bool unbanMember(std::string server_id, std::string member_id);                                                       //to do test this later
		//getRoles  make role array
		Role createRole(std::string server_id);                                                                                //to do test this
		//int editRole(std::string role_id, std::string nam.e = "", int permissions = NULL, int position = -1, int color = , bool hoist =);
		//edit role position
		//editing roles
		bool deleteRole(std::string server_id, std::string role_id);                                                          //to do test this
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
		virtual void onResponse(Response response);
		virtual void sleep(const unsigned int milliseconds);
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

#ifndef SLEEPY_ONE_THREAD
		std::thread clock_thread;
		void runClock_thread();
#endif

		bool updateRateLimiter(const uint8_t numOfMessages = 1);   //returns true when you hit the limit

		std::unique_ptr<std::string> token;		//stored in a unique_ptr so that you can't see it in the debugger
		std::string session_id;
		void getTheGateway();
		char theGateway[32];
		bool ready;
		void sendIdentity();
		void sendResume();
		bool restart();		//it's like start but when it already started
		void reconnect(const unsigned int status = 1000);
		void disconnectWebsocket(unsigned int code, const std::string reason = "");

		uint8_t messagesRemaining;

		//checks to make sure this is valid client
		char magic[6];
		inline bool isMagicReal();

		//error handling
		void setError(int errorCode);

		//templates for discord objects
		template <class _DiscordObject>
		_DiscordObject request(const RequestMethod method, std::string _url, std::string jsonParameters = "") {
			std::string source = request(method, _url, jsonParameters).text;
			return _DiscordObject(&source);
		}

		template <class _DiscordObject>
		std::vector<_DiscordObject> requestVector(const RequestMethod method, std::string _url, std::string jsonParameters = "") {
			std::vector<_DiscordObject> target;
			const std::string source = request(method, _url, jsonParameters).text;
			JSON_getArray<_DiscordObject>(&source, &target);
			return target;
		}

		//class for events
		struct Event {
			const std::string t;
			typedef const void(*EventFunction)(std::string * jsonMessage);
			EventFunction function;
		};
		std::vector<Event> events;

		//time
		const int64_t getEpochTimeMillisecond();
	};
}
