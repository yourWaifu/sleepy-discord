#pragma once
#include <string>
#ifndef SLEEPY_ONE_THREAD
#include <thread>
#endif
#include <memory>
#include <unordered_map>
#include <functional>
#include <forward_list>

//objects
#include "message.h"
#include "channel.h"
#include "server.h"
#include "invite.h"
#include "webhook.h"
#include "permissions.h"
#include "gateway.h"
#include "voice.h"

#include "error.h"
#include "session.h"
#include "common_return_types.h"

/*

remember to make function perimeters const

*/

namespace SleepyDiscord {
#define TOKEN_SIZE 64

	struct Request;

	struct Timer {
	public:
		typedef std::function<void()> StopTimerFunction;
		Timer() {}
		Timer(StopTimerFunction stopTimer) : implStop(stopTimer) {}
		inline void stop() const { implStop(); }
		inline bool isValid() const { return implStop != nullptr; }
	private:
		StopTimerFunction implStop;
	};

	class Route {
	public:
		Route(const std::string route, const std::initializer_list<std::string>& _values = {});
		Route(const char* route);
		inline const std::string& url() {
			return _url;
		}
		const std::string bucket(RequestMethod method);
		inline operator const std::string&() {
			return url();
		}
	private:
		const std::string path;
		std::string _url;
		const std::initializer_list<std::string>& values;
		//major parameters
		Snowflake<Channel> channelID;
		Snowflake<Server> serverID;
	};

	class BaseDiscordClient {
	public:
		BaseDiscordClient();
		BaseDiscordClient(const std::string _token) { start(_token); }
		~BaseDiscordClient();

		Response request(const RequestMethod method, Route path, const std::string jsonParameters = ""/*,
			cpr::Parameters httpParameters = cpr::Parameters{}*/, const std::initializer_list<Part>& multipartParameters = {});
		Response request(const RequestMethod method, Route path, const std::initializer_list<Part>& multipartParameters);
		/*Response request(const RequestMethod method, std::string url, cpr::Parameters httpParameters);*/

		const Route path(const char* source, std::initializer_list<std::string> values = {});

		//channel functions
		ObjectResponse<Channel     > getChannel              (Snowflake<Channel> channelID);
		ObjectResponse<Channel     > editChannel             (Snowflake<Channel> channelID, std::string name = "", std::string topic = "");
		ObjectResponse<Channel     > editChannelName         (Snowflake<Channel> channelID, std::string name);
		ObjectResponse<Channel     > editChannelTopic        (Snowflake<Channel> channelID, std::string topic);
		ObjectResponse<Channel     > deleteChannel           (Snowflake<Channel> channelID);
		enum GetMessagesKey {na, around, before, after, limit};
		ArrayResponse <Message     > getMessages             (Snowflake<Channel> channelID, GetMessagesKey when, Snowflake<Message> messageID, uint8_t limit = 0);
		ObjectResponse<Message     > getMessage              (Snowflake<Channel> channelID, Snowflake<Message> messageID);                                                    //to do add more then one message return
		ObjectResponse<Message     > sendMessage             (Snowflake<Channel> channelID, std::string message, bool tts = false);
		ObjectResponse<Message     > uploadFile              (Snowflake<Channel> channelID, std::string fileLocation, std::string message);
		bool                         addReaction             (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji);
		bool                         removeReaction          (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, Snowflake<User> userID = "@me");
		ArrayResponse <Reaction    > getReactions            (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji);
		void                         removeAllReactions      (Snowflake<Channel> channelID, Snowflake<Message> messageID);
		ObjectResponse<Message     > editMessage             (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string newMessage);
		bool                         deleteMessage           (Snowflake<Channel> channelID, Snowflake<Message> messageID);
		bool                         bulkDeleteMessages      (Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messageIDs);
		/*allow is a bitwise value of all allowed permissions
		deny is a bitwise value of all deisallowed permissions
		type is "member" for a user or "role" for a role*/
		bool                         editChannelPermissions  (Snowflake<Channel> channelID, Snowflake<Overwrite> overwriteID, int allow, int deny, std::string type);           //to do test this
		ArrayResponse <Invite      > getChannelInvites       (Snowflake<Channel> channelID);
		ObjectResponse<Invite      > createChannelInvite     (Snowflake<Channel> channelID, const uint64_t maxAge = 0, const uint64_t maxUses = 0, const bool temporary = false, const bool unique = false);
		bool                         removeChannelPermission (Snowflake<Channel> channelID, std::string ID);
		bool                         sendTyping              (Snowflake<Channel> channelID);
		ArrayResponse <Message     > getPinnedMessages       (Snowflake<Channel> channelID);
		bool                         pinMessage              (Snowflake<Channel> channelID, Snowflake<Message> messageID);
		bool                         unpinMessage            (Snowflake<Channel> channelID, Snowflake<Message> messageID);
		void                         addRecipient            (Snowflake<Channel> channelID, Snowflake<User> userID);
		void                         removeRecipient         (Snowflake<Channel> channelID, Snowflake<User> userID);
		//IntelliSense Help
		/*functions with more then one name to make life easy for users that use IntelliSense*/
		inline bool                  deleteReaction          (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji) { return removeReaction(channelID, messageID, emoji); }
		inline void                  deleteAllReactions      (Snowflake<Channel> channelID, Snowflake<Message> messageID) { removeAllReactions(channelID, messageID); }
		inline bool                  deleteChannelPermission (Snowflake<Channel> channelID, std::string ID) { return removeChannelPermission(channelID, ID); }
		inline void                  deleteRecipient         (Snowflake<Channel> channelID, Snowflake<User> userID) { removeRecipient(channelID, userID); }
		//For Convenience
		inline ObjectResponse<Message> editMessage(Message message, std::string newMessage) { return editMessage(message.channelID, message.ID, newMessage); }

		//server functions
		//ObjectResponse<Server      > createServer            (std::string name, std::string region, std::string icon, int verificationLevel, int defaultMessageNotifications, int explicitContentLevel, std::vector<Role> roles, std::vector<Channel> channels);
		ObjectResponse<Server      > getServer               (Snowflake<Server> serverID);
		//edit Server		//ask discord api server about what the default values should be
		ObjectResponse<Server      > deleteServer            (Snowflake<Server> serverID);
		ArrayResponse <Channel     > getServerChannels       (Snowflake<Server> serverID);
		ObjectResponse<Channel     > createTextChannel       (Snowflake<Server> serverID, std::string name);
		ArrayResponse <Channel     > editChannelPositions    (Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions);
		ObjectResponse<ServerMember> getMember               (Snowflake<Server> serverID, Snowflake<User> userID);
		ArrayResponse <ServerMember> listMembers             (Snowflake<Server> serverID, uint16_t limit = 0, std::string after = "");
		ObjectResponse<ServerMember> addMember               (Snowflake<Server> serverID, Snowflake<User> userID, std::string accesToken, std::string nick = "", std::vector<Role> roles = {}, bool mute = false, bool deaf = false); //to do test this
		bool                         editMember              (Snowflake<Server> serverID, Snowflake<User> userID, std::string nickname = "", std::vector<Snowflake<Role>> roles = {}, int8_t mute = -1, int8_t deaf = -1, Snowflake<Channel> channelID = {});
		bool                         muteServerMember        (Snowflake<Server> serverID, Snowflake<User> userID, bool mute = true);                                  //to do test this
		bool                         editNickname            (Snowflake<Server> serverID, std::string newNickname);
		bool                         addRole                 (Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID);
		bool                         removeRole              (Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID);   //removes role from member
		bool                         kickMember              (Snowflake<Server> serverID, Snowflake<User> userID);
		ArrayResponse <User        > getBans                 (Snowflake<Server> serverID);                                                                     //to do test this
		bool                         banMember               (Snowflake<Server> serverID, Snowflake<User> userID);
		bool                         unbanMember             (Snowflake<Server> serverID, Snowflake<User> userID);
		ArrayResponse <Role        > getRoles                (Snowflake<Server> serverID);
		ObjectResponse<Role        > createRole              (Snowflake<Server> serverID, std::string name = "", Permission permissions = Permission::NONE, unsigned int color = 0, bool hoist = false, bool mentionable = false);
		ArrayResponse <Role        > editRolePosition        (Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions);                //to do test this
		std::string                  editRole                (Snowflake<Server> serverID, Snowflake<Role> roleID, std::string name = "", Permission permissions = Permission::NONE, uint32_t color = 1 << 24, int8_t hoist = -1, int8_t mentionable = -1);
		bool                         deleteRole              (Snowflake<Server> serverID, Snowflake<Role> roleID);
		//get prune count	needs testing to know what object they are talking about
		void                         pruneMembers            (Snowflake<Server> serverID, const unsigned int numOfDays);                                               //to do test
		ArrayResponse <VoiceRegion > getVoiceRegions         ();  //needs voice region class
		ArrayResponse <Invite      > getServerInvites        (Snowflake<Server> serverID);
		std::string                  getIntegrations         (Snowflake<Server> serverID);    //needs whatever a integration class is                        //to do test
		bool                         createIntegration       (Snowflake<Server> serverID, std::string type, std::string integrationID);                          //to do test
		bool                         editIntergration        (Snowflake<Server> serverID, std::string integrationID, int expireBegavior, int expireGracePeriod, bool enbleEmoticons); //to do test
		bool                         deleteIntegration       (Snowflake<Server> serverID, std::string integrationID);                                            //to do test this
		bool                         syncIntegration         (Snowflake<Server> serverID, std::string integrationID);                                              //to do test this
		ObjectResponse<ServerEmbed > getServerEmbed          (Snowflake<Server> serverID);
		//edit server embed   I don't know what the perms are

		//Invite functions
		ObjectResponse<Invite      > inviteEndpoint          (RequestMethod method, std::string inviteCode);
		ObjectResponse<Invite      > getInvite               (std::string inviteCode);                                                                //to do test this
		ObjectResponse<Invite      > deleteInvite            (std::string inviteCode);                                                             //to do test this
		ObjectResponse<Invite      > acceptInvite            (std::string inviteCode);	//not available to bot accounts                          //to do test this

		//User functions
		ObjectResponse<User        > getCurrentUser();
		ObjectResponse<User        > getUser                 (Snowflake<User> userID);
		//User editCurrentUser();		//needs Avatar data thing?
		ArrayResponse <Server>     getServers                ();
		bool                       leaveServer               (Snowflake<Server> serverID);
		ArrayResponse <DMChannel  > getDirectMessageChannels  ();
		ObjectResponse<DMChannel  > createDirectMessageChannel(std::string recipientID);
		//ObjectResponse<DMChannel > createGroupDirectMessageChannel(std:vector<std::string> accessTokens, )   what is a dict???
		ArrayResponse <Connection> getUserConnections        ();

		//Voice Functions
		//getVoiceRegions

		//Webhook functions
		ObjectResponse<Webhook> createWebhook                (Snowflake<Channel> channelID, std::string name, std::string avatar = "");          //to do test this
		ArrayResponse <Webhook> getChannelWebhooks           (Snowflake<Channel> channelID);
		ArrayResponse <Webhook> getServerWebhooks            (Snowflake<Server> serverID);
		ObjectResponse<Webhook> getWebhook                   (Snowflake<Webhook> webhookID, std::string webhookToken = "");                         //to do test this
		ObjectResponse<Webhook> editWebhook                  (Snowflake<Webhook> webhookID, std::string webhookToken = "", std::string name = "", std::string avatar = "");    //you can leave token or name as null //to do test this
		bool                    deleteWebhook                (Snowflake<Webhook> webhookID, std::string webhookToken = "");
		ObjectResponse<Webhook> requestExecuteWebhook        (Snowflake<Webhook> webhookID, std::string webhookToken, std::pair<std::string, std::string> pair, bool wait, std::string username, std::string avatar_url, bool tts);     //note: it's possiable to have both a file and embeds
		ObjectResponse<Webhook> executeWebhook               (Snowflake<Webhook> webhookID, std::string webhookToken, std::string content, bool wait = false, std::string username = "", std::string avatar_url = "", bool tts = false);       //to do test this
		ObjectResponse<Webhook> executeWebhook               (Snowflake<Webhook> webhookID, std::string webhookToken, std::vector<Embed> embeds, bool wait = false, std::string username = "", std::string avatar_url = "", bool tts = false); //to do test this
		ObjectResponse<Webhook> executeWebhook               (Snowflake<Webhook> webhookID, std::string webhookToken, filePathPart file, bool wait = false, std::string username = "", std::string avatar_url = "", bool tts = false);         //to do test this

		//websocket functions
		void updateStatus(std::string gameName = "", uint64_t idleSince = 0);

		void waitTilReady();  ////Deprecated, uses sleep. No replacment for now
		const bool isReady() { return ready; }
		const bool isQuiting() { return quiting; }
		const bool isBot() { return bot; }
		const bool isRateLimited() { return messagesRemaining <= 0 || request(Get, "gateway").statusCode == TOO_MANY_REQUESTS; }
		void setShardID(int _shardID, int _shardCount); //Note: must be called before run or reconnect
		void quit() { quit(false); }	//public function for diconnecting
		virtual void run();

		//time
		enum AssignmentType : bool {
			TilDueTime = 0,
			EpochTime  = 1,
		};
		typedef std::function<void()> TimedTask;
		virtual Timer  schedule(TimedTask                 code   , const time_t millisecondsTilDueTime) { return Timer([](){}); }
		inline  Timer  schedule(TimedTask                 code   , const time_t milliseconds, AssignmentType mode) {
			return     schedule(code, mode == TilDueTime ? milliseconds : milliseconds - getEpochTimeMillisecond());
		}
		inline  Timer  schedule(void (BaseDiscordClient::*code)(), const time_t milliseconds, AssignmentType mode = TilDueTime) {
			return     schedule(std::bind(code, this), milliseconds, mode);
		}
		inline  void  unschedule(const Timer& timer) const { timer.stop(); }
	protected:
		//Rest events
		virtual void onDepletedRequestSupply(time_t timeTilRetry, Request request);
		virtual void onExceededRateLimit(bool global, time_t timeTilRetry, Request request);

		/* list of events
		READY
		RESUMED
		GUILD_CREATE
		GUILD_DELETE
		GUILD_UPDATE
		GUILD_BAN_ADD
		GUILD_BAN_REMOVE
		GUILD_EMOJIS_UPDATE
		*GUILD_INTEGRATIONS_UPDATE
		GUILD_MEMBER_ADD
		GUILD_MEMBER_REMOVE
		GUILD_MEMBER_UPDATE
		GUILD_MEMBERS_CHUNK
		GUILD_ROLE_CREATE
		GUILD_ROLE_UPDATE
		GUILD_ROLE_DELETE
		CHANNEL_CREATE
		CHANNEL_DELETE
		CHANNEL_UPDATE
		CHANNEL_PINS_UPDATE
		PRESENCE_UPDATE
		*PRESENCES_REPLACE
		USER_UPDATE
		USER_NOTE_UPDATE
		USER_SETTINGS_UPDATE
		VOICE_STATE_UPDATE
		TYPING_START
		MESSAGE_CREATE
		MESSAGE_UPDATE
		MESSAGE_DELETE
		MESSAGE_DELETE_BULK
		MESSAGE_REACTION_ADD
		MESSAGE_REACTION_REMOVE
		MESSAGE_REACTION_REMOVE_ALL
		VOICE_SERVER_UPDATE
		GUILD_SYNC
		RELATIONSHIP_ADD
		RELATIONSHIP_REMOVE
		* event that hasn't been added
		*/
		virtual void onReady             (std::string* jsonMessage);
		virtual void onResumed           (std::string* jsonMessage);
		virtual void onDeleteServer      (std::string* jsonMessage);
		virtual void onEditServer        (std::string* jsonMessage);
		virtual void onBan               (std::string* jsonMessage);
		virtual void onUnban             (std::string* jsonMessage);
		virtual void onMember            (std::string* jsonMessage);
		virtual void onRemoveMember      (std::string* jsonMessage);
		virtual void onDeleteMember      (std::string* jsonMessage);
		virtual void onEditMember        (std::string* jsonMessage);
		virtual void onRole              (std::string* jsonMessage);
		virtual void onDeleteRole        (std::string* jsonMessage);
		virtual void onEditRole          (std::string* jsonMessage);
		virtual void onEditEmojis        (std::string* jsonMessage);
		virtual void onMemberChunk       (std::string* jsonMessage);
		virtual void onDeleteChannel     (std::string* jsonMessage);
		virtual void onEditChannel       (std::string* jsonMessage);
		virtual void onPinMessage        (std::string* jsonMessage);
		virtual void onPresenceUpdate    (std::string* jsonMessage);
		virtual void onEditUser          (std::string* jsonMessage);
		virtual void onEditUserNote      (std::string* jsonMessage);
		virtual void onEditUserSettings  (std::string* jsonMessage);
		virtual void onEditVoiceState    (std::string* jsonMessage);
		virtual void onTyping            (std::string* jsonMessage);
		virtual void onDeleteMessage     (std::string* jsonMessage);
		virtual void onEditMessage       (std::string* jsonMessage);
		virtual void onBulkDelete        (std::string* jsonMessage);
		virtual void onEditVoiceServer   (std::string* jsonMessage);
		virtual void onServerSync        (std::string* jsonMessage);
		virtual void onRelationship      (std::string* jsonMessage);
		virtual void onRemoveRelationship(std::string* jsonMessage);
		virtual void onDeleteRelationship(std::string* jsonMessage);
		virtual void onReaction          (std::string* jsonMessage);
		virtual void onRemoveReaction    (std::string* jsonMessage);
		virtual void onDeleteReaction    (std::string* jsonMessage);
		virtual void onRemoveAllReaction (std::string* jsonMessage);
		virtual void onDeleteAllReaction (std::string* jsonMessage);
		virtual void onMessage           (Message message         );
		virtual void onEditedMessage     (std::string* jsonMessage);
		virtual void onServer            (Server server           );
		virtual void onChannel           (std::string* jsonMessage);
		virtual void onEditedRole        (std::string* jsonMessage);
		virtual void onDispatch          (std::string* jsonMessage);

		//websocket stuff
		virtual void onHeartbeat();
		virtual void onHeartbeatAck();
		virtual void onInvaldSession();
		virtual void onDisconnect();
		virtual void onResume();

		virtual void onQuit();
		virtual void onResponse(Response response);
		virtual void sleep(const unsigned int milliseconds);  //Deprecated, use schedule instead
		virtual void fileRead(const char* path, std::string*const file);
		virtual void tick(float deltaTime);
		virtual void onError(ErrorCode errorCode, const std::string errorMessage);

		/*do not use or overwrite the protected values below,
		unless you know what you are doing*/
		void processMessage(std::string message);
		void processCloseCode(const int16_t code);
		void heartbeat();
		void sendHeartbeat();
		void resetHeartbeatValues();
		inline std::string getToken() { return *token.get(); }
		void start(const std::string _token, const char maxNumOfThreads = 2, int _shardID = 0, int _shardCount = 0);
		virtual bool connect(const std::string & uri) { return false; }
		void handleFailToConnect() { schedule([=]() { reconnect(); }, 10000); }
		virtual void send(std::string message) {}
		virtual void disconnect(unsigned int code, const std::string reason) {}
		void reconnect(const unsigned int status = 1000);
		virtual void runAsync();
		virtual const time_t getEpochTimeMillisecond();
		
	private:
		int heartbeatInterval = 0;
		int64_t lastHeartbeat = 0;
		int lastSReceived = 0;
		bool wasHeartbeatAcked = true;
		Timer heart;

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

		std::unique_ptr<std::string> token;		//stored in a unique_ptr so that you can't see it in the debugger
		std::string sessionID;
		int shardID;
		int shardCount;
		void getTheGateway();
		std::string theGateway;
		bool ready;
		bool quiting;
		bool bot;
		int consecutiveReconnectsCount = 0;
		Timer reconnectTimer;
		void sendIdentity();
		void sendResume();
		//bool restart();		//it's like start but when it already started. it's basicly useless in it's current form
		void quit(bool isRestarting, bool isDisconnected = false);
		void disconnectWebsocket(unsigned int code, const std::string reason = "");
		bool sendL(std::string message);    //the L stands for Limited
		int64_t nextHalfMin = 0;

		//Cache
		std::shared_ptr<ServerCache> serverCache;

		//rate limiting
		int8_t messagesRemaining;
		bool isGlobalRateLimited = false;
		time_t nextRetry = 0;
		std::unordered_map<std::string, time_t> buckets;

		//error handling
		void setError(int errorCode);

		//for endpoint functions
		const std::string getEditPositionString(const std::vector<std::pair<std::string, uint64_t>>& positions);
	};

	/*Used when you like to have the DiscordClient to handle the timer via a loop but 
	  don't want to do yourself. I plan on somehow merging this with the baseClient
	  somehow

	  This is here temporarily until the DiscordClient is overhauled
	  */
	class AssignmentBasedDiscordClient : public BaseDiscordClient {
	public:
		Timer schedule(TimedTask code, const time_t milliseconds);
	protected:
		void resumeMainLoop();
		virtual int setDoAssignmentTimer(const time_t milliseconds); //call doAssignment in x milliseconds
		virtual void stopDoAssignmentTimer(const int jobID) {}
		void doAssignment();
	private:
		struct Assignment {
			int jobID;
			TimedTask function;
			time_t dueTime;
		};
		std::forward_list<Assignment> assignments;

		void unschedule(const int jobID);
	};

	struct Request {
		BaseDiscordClient& client;
		const RequestMethod method;
		const std::string url;
		const std::string jsonParameters;
		const std::initializer_list<Part> multipartParameters;
		inline void operator()() {
			client.request(method, url, jsonParameters, multipartParameters);
		}
		//inline operator std::function<void()>() {
		//	return std::bind(&Request::operator(), this);
		//}
	};

}

//locks away functions that users shouldn't be using
#define SLEEPY_LOCK_CLIENT_FUNCTIONS private:                                  \
                                     using BaseDiscordClient::processMessage;  \
                                     using BaseDiscordClient::start;           \
                                     using BaseDiscordClient::sendHeartbeat;   \
                                     using BaseDiscordClient::processCloseCode;\
                                     using BaseDiscordClient::reconnect;       \

//This comment stops a warning