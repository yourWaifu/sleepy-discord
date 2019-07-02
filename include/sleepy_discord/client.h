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
#include "message_receiver.h"
#include "timer.h"
#include "voice_connection.h"
#include "asio_schedule.h"

namespace SleepyDiscord {
#define TOKEN_SIZE 64

	struct Request;

	//Modes
	enum Mode : char {
		USER_CONTROLED_THREADS = 1,
		USE_RUN_THREAD = 3,
		DEFAULT_THREADS = USER_CONTROLED_THREADS
	};

	class Route {
	public:
		using Bucket = std::string;
		Route(const std::string route, const std::initializer_list<std::string>& _values = {});
		Route(const char* route);
		inline const std::string& url() {
			return _url;
		}
		const Bucket bucket(RequestMethod method);
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

	enum RequestMode {
		Async,
		Sync
	};

	class BaseDiscordClient : public GenericMessageReceiver {
	public:
		BaseDiscordClient() = default;
		BaseDiscordClient(const std::string _token) { start(_token); }
		~BaseDiscordClient();

		using RequestCallback = std::function<void(Response)>;
		Response request(const RequestMethod method, Route path, const std::string jsonParameters = ""/*,
			cpr::Parameters httpParameters = cpr::Parameters{}*/, const std::initializer_list<Part>& multipartParameters = {},
			RequestCallback callback = nullptr, RequestMode mode = Sync);
		Response request(const RequestMethod method, Route path, const std::initializer_list<Part>& multipartParameters);
		/*Response request(const RequestMethod method, std::string url, cpr::Parameters httpParameters);*/
		struct Request {
			BaseDiscordClient& client;
			const RequestMethod method;
			const Route url;
			const std::string jsonParameters;
			const std::initializer_list<Part> multipartParameters;
			const BaseDiscordClient::RequestCallback callback;
			inline void operator()() const {
				client.request(method, url, jsonParameters, multipartParameters, callback);
			}
		};

		template<class ParmType, class Callback>
		void requestAsync(const RequestMethod method, Route path, Callback callback, const std::string jsonParameters = "",
			const std::initializer_list<Part>& multipartParameters = {}) {
			postTask(static_cast<PostableTask>(
				Request{ *this, method, path, jsonParameters, multipartParameters, [callback](Response r) {
					callback(static_cast<ParmType>(r));
				} }
			));
		}

		template<class ParmType, class Callback>
		Response requestSync(const RequestMethod method, Route path, Callback callback, const std::string jsonParameters = "",
			const std::initializer_list<Part>& multipartParameters = {}) {
			return request(method, path, jsonParameters, multipartParameters, [callback](Response r) {
				callback(static_cast<ParmType>(r));
			});
		}

		const Route path(const char* source, std::initializer_list<std::string> values = {});

#ifndef SLEEPY_DEFAULT_REQUEST_MODE
	#ifdef SLEEPY_DEFAULT_REQUEST_MODE_ASYNC
		#define SLEEPY_DEFAULT_REQUEST_MODE Async;
	#elif defined(SLEEPY_DEFAULT_REQUEST_MODE_SYNC)
		#define SLEEPY_DEFAULT_REQUEST_MODE Sync;
	#else
		#define SLEEPY_DEFAULT_REQUEST_MODE Sync;
	#endif
#endif

		template<class _ParmType>
		struct RequestSettings {
			RequestMode mode = SLEEPY_DEFAULT_REQUEST_MODE;
			using ParmType = _ParmType;
			using Callback = std::function<void(ParmType)>;
			Callback callback = nullptr;

			RequestSettings(RequestMode r) : mode(r) {}
			RequestSettings(Callback c) : mode(Async), callback(c) {}
			RequestSettings(RequestMode r, Callback c) : mode(r), callback(c) {}
			RequestSettings() {}; //for some reason = default doesn't work
		};

		template<class RequestSettingsClass>
		Response request(const RequestMethod method, Route path, RequestSettingsClass& settings,
			const std::string jsonParameters = "", const std::initializer_list<Part>& multipartParameters = {}) {
			switch (settings.mode) {
			case Async:
				requestAsync<
					typename RequestSettingsClass::ParmType, typename RequestSettingsClass::Callback
				>(method, path, settings.callback, jsonParameters, multipartParameters);
				break;
			case Sync:
				if (settings.callback)
					//having an invalid callback here would cause bugs
					return requestSync<
						typename RequestSettingsClass::ParmType, typename RequestSettingsClass::Callback
					>(method, path, settings.callback, jsonParameters, multipartParameters);
				else
					return request(method, path, jsonParameters, multipartParameters);
				break;
			default:
				return Response(BAD_REQUEST);
				break;
			}
			return Response();
		}

		template<RequestMode mode, class type>
		struct RawRequestModeTypeHelper {
			using ReturnType = type;
			static type doRequest() {}
		};

#define RequestModeRequestDefine template<class ParmType, class Callback> \
		static ReturnType doRequest(BaseDiscordClient& client, const RequestMethod method, Route path, \
			const std::string jsonParameters, const std::initializer_list<Part>& multipartParameters, Callback callback) 

		template<RequestMode mode> struct RequestModeType : RawRequestModeTypeHelper<Sync, void> {};

		template<RequestMode mode, class ParmType = void, class Callback = RequestCallback>
		typename RequestModeType<mode>::ReturnType request(const RequestMethod method, Route path, Callback callback,
			const std::string jsonParameters = ""/*,
			cpr::Parameters httpParameters = cpr::Parameters{}*/, const std::initializer_list<Part>& multipartParameters = {}
			) {
			return RequestModeType<mode>::template doRequest<ParmType, Callback>(*this, method, path, jsonParameters, multipartParameters, callback);
		}

		template<class RequestSettingsClass>
		typename RequestSettingsClass::ReturnType request(const RequestMethod method, Route path, RequestSettingsClass& settings,
			const std::string jsonParameters = "", const std::initializer_list<Part>& multipartParameters = {}) {
			switch (settings.mode) {
			case Async:        requestAsync<
			                   	typename RequestSettingsClass::ParmType,
			                   	typename RequestSettingsClass::Callback
			                   >(method, path, settings.callback, jsonParameters, multipartParameters); break;
			case Sync:  return requestSync                     (method, path, settings.callback, jsonParameters, multipartParameters); break;
			default:    return Response(BAD_REQUEST); break;
			}
			return Response();
		}

		void testFunction(std::string teststring);

		//channel functions
		ObjectResponse<Channel     > getChannel              (Snowflake<Channel> channelID, RequestSettings<ObjectResponse<Channel>> settings = {});
		ObjectResponse<Channel     > editChannel             (Snowflake<Channel> channelID, std::string name = "", std::string topic = "");
		ObjectResponse<Channel     > editChannelName         (Snowflake<Channel> channelID, std::string name);
		ObjectResponse<Channel     > editChannelTopic        (Snowflake<Channel> channelID, std::string topic);
		ObjectResponse<Channel     > deleteChannel           (Snowflake<Channel> channelID);
		enum GetMessagesKey {na, around, before, after, limit};
		ArrayResponse <Message     > getMessages             (Snowflake<Channel> channelID, GetMessagesKey when, Snowflake<Message> messageID, uint8_t limit = 0);
		ObjectResponse<Message     > getMessage              (Snowflake<Channel> channelID, Snowflake<Message> messageID);                                                    //to do add more then one message return
		ObjectResponse<Message     > sendMessage             (Snowflake<Channel> channelID, std::string message, Embed embed = Embed::Flag::INVALID_EMBED, bool tts = false);
		ObjectResponse<Message     > uploadFile              (Snowflake<Channel> channelID, std::string fileLocation, std::string message);
		BoolResponse                 addReaction             (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji);
		BoolResponse                 removeReaction          (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, Snowflake<User> userID = "@me");
		ArrayResponse <Reaction    > getReactions            (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji);
		StandardResponse             removeAllReactions      (Snowflake<Channel> channelID, Snowflake<Message> messageID);
		ObjectResponse<Message     > editMessage             (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string newMessage);
		BoolResponse                 deleteMessage           (Snowflake<Channel> channelID, Snowflake<Message> messageID);
		BoolResponse                 bulkDeleteMessages      (Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messageIDs);
		/*allow is a bitwise value of all allowed permissions
		deny is a bitwise value of all deisallowed permissions
		type is "member" for a user or "role" for a role*/
		BoolResponse                 editChannelPermissions  (Snowflake<Channel> channelID, Snowflake<Overwrite> overwriteID, int allow, int deny, std::string type);           //to do test this
		ArrayResponse <Invite      > getChannelInvites       (Snowflake<Channel> channelID);
		ObjectResponse<Invite      > createChannelInvite     (Snowflake<Channel> channelID, const uint64_t maxAge = 0, const uint64_t maxUses = 0, const bool temporary = false, const bool unique = false);
		BoolResponse                 removeChannelPermission (Snowflake<Channel> channelID, std::string ID);
		BoolResponse                 sendTyping              (Snowflake<Channel> channelID);
		ArrayResponse <Message     > getPinnedMessages       (Snowflake<Channel> channelID);
		BoolResponse                 pinMessage              (Snowflake<Channel> channelID, Snowflake<Message> messageID);
		BoolResponse                 unpinMessage            (Snowflake<Channel> channelID, Snowflake<Message> messageID);
		StandardResponse             addRecipient            (Snowflake<Channel> channelID, Snowflake<User> userID);
		StandardResponse             removeRecipient         (Snowflake<Channel> channelID, Snowflake<User> userID);
		//IntelliSense Help
		/*functions with more then one name to make life easy for users that use IntelliSense*/
		inline BoolResponse          deleteReaction          (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji) { return removeReaction         (channelID, messageID, emoji); }
		inline StandardResponse      deleteAllReactions      (Snowflake<Channel> channelID, Snowflake<Message> messageID                   ) { return removeAllReactions     (channelID, messageID       ); }
		inline BoolResponse          deleteChannelPermission (Snowflake<Channel> channelID, std::string               ID                   ) { return removeChannelPermission(channelID,        ID       ); }
		inline StandardResponse      deleteRecipient         (Snowflake<Channel> channelID, Snowflake<User   >    _userID                  ) { return removeRecipient        (channelID,   _userID       ); }
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
		BoolResponse                 editMember              (Snowflake<Server> serverID, Snowflake<User> userID, std::string nickname = "", std::vector<Snowflake<Role>> roles = {}, int8_t mute = -1, int8_t deaf = -1, Snowflake<Channel> channelID = {});
		BoolResponse                 muteServerMember        (Snowflake<Server> serverID, Snowflake<User> userID, bool mute = true);                                  //to do test this
		BoolResponse                 editNickname            (Snowflake<Server> serverID, std::string newNickname);
		BoolResponse                 addRole                 (Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID);
		BoolResponse                 removeRole              (Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID);   //removes role from member
		BoolResponse                 kickMember              (Snowflake<Server> serverID, Snowflake<User> userID);
		ArrayResponse <User        > getBans                 (Snowflake<Server> serverID);                                                                     //to do test this
		BoolResponse                 banMember               (Snowflake<Server> serverID, Snowflake<User> userID);
		BoolResponse                 unbanMember             (Snowflake<Server> serverID, Snowflake<User> userID);
		ArrayResponse <Role        > getRoles                (Snowflake<Server> serverID);
		ObjectResponse<Role        > createRole              (Snowflake<Server> serverID, std::string name = "", Permission permissions = Permission::NONE, unsigned int color = 0, bool hoist = false, bool mentionable = false);
		ArrayResponse <Role        > editRolePosition        (Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions);                //to do test this
		StringResponse               editRole                (Snowflake<Server> serverID, Snowflake<Role> roleID, std::string name = "", Permission permissions = Permission::NONE, uint32_t color = 1 << 24, int8_t hoist = -1, int8_t mentionable = -1);
		BoolResponse                 deleteRole              (Snowflake<Server> serverID, Snowflake<Role> roleID);
		//get prune count	needs testing to know what object they are talking about
		StandardResponse             pruneMembers            (Snowflake<Server> serverID, const unsigned int numOfDays);                                               //to do test
		ArrayResponse <VoiceRegion > getVoiceRegions         ();  //needs voice region class
		ArrayResponse <Invite      > getServerInvites        (Snowflake<Server> serverID);
		StringResponse               getIntegrations         (Snowflake<Server> serverID);    //needs whatever a integration class is                        //to do test
		BoolResponse                 createIntegration       (Snowflake<Server> serverID, std::string type, std::string integrationID);                          //to do test
		BoolResponse                 editIntergration        (Snowflake<Server> serverID, std::string integrationID, int expireBegavior, int expireGracePeriod, bool enbleEmoticons); //to do test
		BoolResponse                 deleteIntegration       (Snowflake<Server> serverID, std::string integrationID);                                            //to do test this
		BoolResponse                 syncIntegration         (Snowflake<Server> serverID, std::string integrationID);                                              //to do test this
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
		BoolResponse               leaveServer               (Snowflake<Server> serverID);
		ArrayResponse <Channel   > getDirectMessageChannels  ();
		ObjectResponse<Channel   > createDirectMessageChannel(std::string recipientID);
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
		BoolResponse            deleteWebhook                (Snowflake<Webhook> webhookID, std::string webhookToken = "");
		ObjectResponse<Webhook> requestExecuteWebhook        (Snowflake<Webhook> webhookID, std::string webhookToken, std::pair<std::string, std::string> pair, bool wait, std::string username, std::string avatar_url, bool tts);     //note: it's possiable to have both a file and embeds
		ObjectResponse<Webhook> executeWebhook               (Snowflake<Webhook> webhookID, std::string webhookToken, std::string content, bool wait = false, std::string username = "", std::string avatar_url = "", bool tts = false);       //to do test this
		ObjectResponse<Webhook> executeWebhook               (Snowflake<Webhook> webhookID, std::string webhookToken, std::vector<Embed> embeds, bool wait = false, std::string username = "", std::string avatar_url = "", bool tts = false); //to do test this
		ObjectResponse<Webhook> executeWebhook               (Snowflake<Webhook> webhookID, std::string webhookToken, filePathPart file, bool wait = false, std::string username = "", std::string avatar_url = "", bool tts = false);         //to do test this

		//websocket functions
		void updateStatus(std::string gameName = "", uint64_t idleSince = 0, Status status = online, bool afk = false);

		void waitTilReady();  ////Deprecated, uses sleep. No replacment for now
		const bool isReady() { return ready; }
		const bool isQuiting() { return quiting; }
		const bool isBot() { return bot; }
		const Snowflake<User> getID() { return userID; }
		void setShardID(int _shardID, int _shardCount); //Note: must be called before run or reconnect
		const int getShardID() { return shardID; }
		const int getShardCount() { return shardCount; }
		void quit() { quit(false); }	//public function for diconnecting
		virtual void run();
		
		//time
		template <class Handler, class... Types>
		inline void setScheduleHandler(Types&&... arguments) {
			scheduleHandler = std::unique_ptr<GenericScheduleHandler>(
				new Handler(std::forward<Types>(arguments)...)
			);
		}
		inline GenericScheduleHandler& getScheduleHandler() { return *scheduleHandler; } 

		enum AssignmentType : bool {
			TilDueTime = 0,
			EpochTime  = 1,
		};
		virtual Timer  schedule(TimedTask                 code   , const time_t millisecondsTilDueTime);
		inline  Timer  schedule(TimedTask                 code   , const time_t milliseconds, AssignmentType mode) {
			return     schedule(code, mode == TilDueTime ? milliseconds : milliseconds - getEpochTimeMillisecond());
		}
		inline  Timer  schedule(void (BaseDiscordClient::*code)(), const time_t milliseconds, AssignmentType mode = TilDueTime) {
			return     schedule(std::bind(code, this), milliseconds, mode);
		}
		inline  void  unschedule(const Timer& timer) const { timer.stop(); }
		
		typedef TimedTask PostableTask;
		virtual void postTask(PostableTask code) {
			schedule(code, 0);
		}

#ifdef SLEEPY_VOICE_ENABLED
		//
		//voice
		//

		friend VoiceConnection;

		enum VoiceMode {
			normal = 0,
			mute = 1 << 0,
			deafen = 1 << 1
		};

		VoiceContext& createVoiceContext(Snowflake<Server> server, Snowflake<Channel> channel, BaseVoiceEventHandler* eventHandler = nullptr);
		inline VoiceContext& createVoiceContext(Snowflake<Channel> channel, BaseVoiceEventHandler* eventHandler = nullptr) {
			return createVoiceContext("", channel, eventHandler);
		}
		void connectToVoiceChannel(VoiceContext& voiceContext, VoiceMode settings);
		VoiceContext& connectToVoiceChannel(Snowflake<Server> server, Snowflake<Channel> channel, VoiceMode settings = normal);
		VoiceContext& connectToVoiceChannel(Snowflake<Channel> channel, VoiceMode settings = normal) {
			return connectToVoiceChannel("", channel, settings);
		}
		inline void disconnectVoiceConnection(VoiceConnection & connection) {
			connection.disconnect();
		}
		
		template<class Function>
		void disconnectVoiceConnection_if(Function function) {
			auto i = std::find_if(voiceConnections.begin(), voiceConnections.end(), function);
			if (i != voiceConnections.end())
				disconnectVoiceConnection(*i);
		}

		inline void disconnectVoiceContext(VoiceContext & context) {
			disconnectVoiceConnection_if([&context](VoiceConnection& connection) {
				return connection.getContext() == context;
			});
		}

		inline void disconnectFromVoiceChannel(Snowflake<Channel>& channelID) {
			disconnectVoiceConnection_if([&channelID](VoiceConnection& connection) {
				return connection.getContext().getChannelID() == channelID;
			});
		}

		inline void disconnectServerVoiceConnections(Snowflake<Server>& serverID) {
			disconnectVoiceConnection_if([&serverID](VoiceConnection& connection) {
				return connection.getContext().getServerID() == serverID;
			});
		}
#endif

		//Caching
		std::shared_ptr<ServerCache> createServerCache();
		void setServerCache(std::shared_ptr<ServerCache> cache);
		inline std::shared_ptr<ServerCache>& getServerCache() {
			return serverCache;
		}

	protected:
		//Rest events
		virtual void onDepletedRequestSupply(const Route::Bucket& bucket, time_t timeTilReset);
		virtual void onExceededRateLimit(bool global, std::time_t timeTilRetry, RequestMode mode, Request request);

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
		//TODO: use references, using them now will break other's code
		virtual void onReady             (Ready              readyData  );
		virtual void onResumed           (const json::Value& jsonMessage);
		virtual void onDeleteServer      (UnavailableServer  server);
		virtual void onEditServer        (Server             server);
		virtual void onBan               (Snowflake<Server> serverID, User user);
		virtual void onUnban             (Snowflake<Server> serverID, User user);
		virtual void onMember            (Snowflake<Server> serverID, ServerMember member);
		virtual void onRemoveMember      (Snowflake<Server> serverID, User user);
		virtual void onEditMember        (Snowflake<Server> serverID, User user, std::vector<Snowflake<Role>> roles, std::string nick);
		virtual void onRole              (Snowflake<Server> serverID, Role role);
		virtual void onDeleteRole        (Snowflake<Server> serverID, Snowflake<Role> roleID);
		virtual void onEditRole          (Snowflake<Server> serverID, Role role);
		virtual void onEditEmojis        (Snowflake<Server> serverID, std::vector<Emoji> emojis);
		virtual void onMemberChunk       (Snowflake<Server> serverID, std::vector<ServerMember> members);
		virtual void onDeleteChannel     (Channel channel);
		virtual void onEditChannel       (Channel channel);
		virtual void onPinMessage        (Snowflake<Channel> channelID, std::string lastPinTimestamp);
		virtual void onPresenceUpdate    (PresenceUpdate     presenseUpdate);
		virtual void onEditUser          (User user);
		virtual void onEditUserNote      (const json::Value& jsonMessage);
		virtual void onEditUserSettings  (const json::Value& jsonMessage);
		virtual void onEditVoiceState    (VoiceState&        state      );
		virtual void onTyping            (Snowflake<Channel> channelID, Snowflake<User> userID, time_t timestamp);
		virtual void onDeleteMessages    (Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messages);
		virtual void onEditMessage       (const json::Value& jsonMessage);
		virtual void onEditVoiceServer   (VoiceServerUpdate& update     );
		virtual void onServerSync        (const json::Value& jsonMessage);
		virtual void onRelationship      (const json::Value& jsonMessage);
		virtual void onDeleteRelationship(const json::Value& jsonMessage);
		virtual void onReaction          (Snowflake<User> userID, Snowflake<Channel> channelID, Snowflake<Message> messageID, Emoji emoji);
		virtual void onDeleteReaction    (Snowflake<User> userID, Snowflake<Channel> channelID, Snowflake<Message> messageID, Emoji emoji);
		virtual void onDeleteAllReaction (Snowflake<Server> serverID, Snowflake<Channel> channelID, Snowflake<Message> messageID);
		virtual void onMessage           (Message            message    );
		virtual void onServer            (Server             server     );
		virtual void onChannel           (Channel            channel    );
		virtual void onDispatch          (const json::Value& jsonMessage);

		//websocket stuff
		virtual void onHeartbeat();
		virtual void onHeartbeatAck();
		virtual void onInvaldSession();
		virtual void onDisconnect();
		virtual void onResume();

		virtual void onQuit();
		virtual void onRestart() {}
		virtual void onResponse(Response response);
		virtual void sleep(const unsigned int milliseconds);  //Deprecated, use schedule instead
		virtual void fileRead(const char* path, std::string*const file);
		virtual void tick(float deltaTime);
		virtual void onError(ErrorCode errorCode, const std::string errorMessage);

		/*do not use or overwrite the protected values below,
		unless you know what you are doing*/
		void processMessage(const std::string &message) override;
		void processCloseCode(const int16_t code) override;
		void heartbeat();
		void sendHeartbeat();
		inline std::string getToken() { return *token.get(); }
		void start(const std::string _token, const char maxNumOfThreads = DEFAULT_THREADS, int _shardID = 0, int _shardCount = 0);
		virtual bool connect(
			const std::string & /*uri*/,                    //IN
			GenericMessageReceiver* /*messageProcessor*/,   //IN  When a message is receved, this will process it
			WebsocketConnection& /*connection*/             //OUT data needed in order to send a message. nullptr by default
		) { return false; }
		void handleFailToConnect() override { reconnect(); }
		virtual void send(std::string /*message*/, WebsocketConnection& /*connection*/) {}
		virtual void disconnect(unsigned int /*code*/, const std::string /*reason*/, WebsocketConnection& /*connection*/) {}
		void reconnect(const unsigned int status = 1000);
		void stopClient() {}
		//the next 3 functions are part of BaseDiscordClient because VoiceConnection is a private nested class
		inline void initialize(GenericMessageReceiver*& messageProcessor) const {
			messageProcessor->initialize();
		}
		inline void processMessage(GenericMessageReceiver*& messageProcessor, const std::string& message) const {
			messageProcessor->processMessage(message);
		}
		inline void processCloseCode(GenericMessageReceiver*& messageProcessor, const int16_t& code) const {
			messageProcessor->processCloseCode(code);
		}
		virtual void runAsync();
		virtual const time_t getEpochTimeMillisecond();

	private:
		using GenericMessageReceiver::initialize;

		int heartbeatInterval = 0;
		int64_t lastHeartbeat = 0;
		int lastSReceived = 0;
		bool wasHeartbeatAcked = true;
		std::unique_ptr<GenericScheduleHandler> scheduleHandler = nullptr;
		Timer heart;

		enum OPCode {
			DISPATCH              = 0,  //dispatches an event
			HEARTHBEAT            = 1,  //used for ping checking
			IDENTIFY              = 2,  //used for client handshake
			STATUS_UPDATE         = 3,  //used to update the client status
			VOICE_STATE_UPDATE    = 4,  //used to join / move / leave voice channels
			VOICE_SERVER_PING     = 5,  //used for voice ping checking
			RESUME                = 6,  //used to resume a closed connection
			RECONNECT             = 7,  //used to tell clients to reconnect to the gateway
			REQUEST_GUILD_MEMBERS = 8,  //used to request guild members
			INVALID_SESSION       = 9,  //used to notify client they have an invalid session id
			HELLO                 = 10, //sent immediately after connecting, contains heartbeat and server debug information
			HEARTBEAT_ACK         = 11, //sent immediately following a client heartbeat that was received
		};

		std::unique_ptr<std::string> token;		//stored in a unique_ptr so that you can't see it in the debugger
		std::string sessionID;	//TODO: replace this with a Ready object
		int shardID = 0;
		int shardCount = 0;
		Snowflake<User> userID;
		void getTheGateway();
		std::string theGateway;
		bool ready = false;
		bool quiting = false;
		bool bot = true;
		int consecutiveReconnectsCount = 0;
		Timer reconnectTimer;
		void sendIdentity();
		void sendResume();
		void quit(bool isRestarting, bool isDisconnected = false);
		void restart();
		void disconnectWebsocket(unsigned int code, const std::string reason = "");
		bool sendL(std::string message);    //the L stands for Limited
		int64_t nextHalfMin = 0;

		//Cache
		std::shared_ptr<ServerCache> serverCache;

		//rate limiting
		int8_t messagesRemaining = 0;
		bool isGlobalRateLimited = false;
		time_t nextRetry = 0;
		std::unordered_map<Route::Bucket, time_t> buckets;

		//error handling
		void setError(int errorCode);

		//for endpoint functions
		const std::string getEditPositionString(const std::vector<std::pair<std::string, uint64_t>>& positions);

		//
		//voice
		//
		std::list<VoiceConnection> voiceConnections;
		std::forward_list<VoiceContext> voiceContexts;
		std::forward_list<VoiceContext*> waitingVoiceContexts;
#ifdef SLEEPY_VOICE_ENABLED
		void connectToVoiceIfReady(VoiceContext& context);
		void removeVoiceConnectionAndContext(VoiceConnection& connection);
#endif

		template<class Callback>
		void findServerInCache(Snowflake<Server>& serverID, Callback onSuccessCallback) {
			if (serverCache) {
				ServerCache::iterator server = serverCache->findServer(serverID);
				if (server != serverCache->end()) {
					onSuccessCallback(server);
				}
			}
		}

		template<class Callback>
		void accessServerFromCache(Snowflake<Server>& serverID, Callback callback) {
			findServerInCache(serverID, [callback](ServerCache::iterator& found) {
				callback(*found);
			});
		}

		template<class Container, class Callback>
		void accessContainerFromCache(
			Snowflake<Server>& serverID, Container Server::* container, Callback callback
		) {
			accessServerFromCache(serverID, [callback, container](Server& server) {
				callback(server, server.*(container));
			});
		}

		template<class Container>
		void appendObjectToCache(
			Snowflake<Server>& serverID, Container Server::* container, typename Container::value_type& object
		) {
			accessContainerFromCache(serverID, container,
				[object](Server&, Container& found) {
					found.push_front(object);
				}
			);
		}

		template<class Type, class Container, class Callback>
		void accessIteratorFromCache (
			Snowflake<Server>& serverID, Container Server::* container, Type ID, Callback callback
		) {
			accessContainerFromCache(serverID, container,
				[callback, ID](Server& server, Container& foundContainter) {
					auto found = ID.findObject(foundContainter);
					if (found != foundContainter.end()) {
						callback(server, found);
					}
				}
			);
		}

		template<class Type, class Container, class Callback>
		void accessObjectFromCache(
			Snowflake<Server> serverID, Container Server::* container, Type ID, Callback callback
		) {
			accessIteratorFromCache(serverID, container, ID,
				[callback] (Server& server, typename Container::iterator& iterator) {
					callback(server, *iterator);
				}
			);
		}

		template<class Type, class Container>
		void eraseObjectFromCache(
			Snowflake<Server> serverID, Container Server::* container, Type ID
		) {
			accessIteratorFromCache(serverID, container, ID, 
				[container](Server& server, typename Container::iterator& found) {
					(server.*(container)).erase(found);
				}
			);
		}
	};

	//inline BaseDiscordClient::AssignmentType operator|(BaseDiscordClient::AssignmentType left, BaseDiscordClient::AssignmentType right) {
	//	return static_cast<BaseDiscordClient::AssignmentType>(static_cast<char>(left) | static_cast<char>(right));
	//}
	//inline BaseDiscordClient::AssignmentType operator&(BaseDiscordClient::AssignmentType left, BaseDiscordClient::AssignmentType right) {
	//	return static_cast<BaseDiscordClient::AssignmentType>(static_cast<char>(left) & static_cast<char>(right));
	//}

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

	template<> struct BaseDiscordClient::RequestModeType<Async> : BaseDiscordClient::RawRequestModeTypeHelper<Async, void> {
		RequestModeRequestDefine {
			client.requestAsync<ParmType, Callback>(method, path, callback, jsonParameters, multipartParameters);
		}
	};
	template<> struct BaseDiscordClient::RequestModeType<Sync>  : BaseDiscordClient::RawRequestModeTypeHelper<Sync , Request> {
		RequestModeRequestDefine {
			return client.requestSync(method, path, callback, jsonParameters, multipartParameters);
		}
	};

}

//locks away functions that users shouldn't be using
#define SLEEPY_LOCK_CLIENT_FUNCTIONS private:                                  \
                                     using BaseDiscordClient::processMessage;  \
                                     using BaseDiscordClient::start;           \
                                     using BaseDiscordClient::sendHeartbeat;   \
                                     using BaseDiscordClient::initialize;      \
                                     using BaseDiscordClient::processCloseCode;\
                                     using BaseDiscordClient::reconnect;       \
                                     using BaseDiscordClient::stopClient;      \

//This comment stops a warning