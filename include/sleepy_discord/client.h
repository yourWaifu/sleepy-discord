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

	//to dos
	//intents
	//custom rapid json error
	//detect cloudflare error
	//emojis rate limits
	//async
	//merge to master
	//cache

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

		//for the snowflake part, discord class should do
		std::unordered_map<std::string, Snowflake<User>::RawType>
			majorParameters = {
			{ "channel.id", {} },
			{ "guild.id"  , {} },
			{ "webhook.id", {} }
		};
	};

	struct RateLimiter {
		std::atomic<bool> isGlobalRateLimited = { false };
		std::atomic<time_t> nextRetry = { 0 };
		void limitBucket(Route::Bucket& bucket, time_t timestamp);
		const time_t getLiftTime(Route::Bucket& bucket, const time_t& currentTime);
		//isLimited also returns the next Retry timestamp
	private:
		std::unordered_map<Route::Bucket, time_t> buckets;
		std::mutex mutex;
	};

	enum RequestMode {
		UseRequestAsync = 1 << 0,
		UseRequestSync = 0 << 0,

		ThrowError = 1 << 4,
		AsyncQueue = 1 << 5,

		Async           = UseRequestAsync | AsyncQueue,
		Sync            = UseRequestSync | ThrowError,
		Sync_AsyncQueue = UseRequestSync | ThrowError | AsyncQueue, //old behavior for backwards compat
	};

	using IntentsRaw = int32_t;

	enum Intent : IntentsRaw {
		SERVERS                  = 1 << 0,
		SERVER_MEMBERS           = 1 << 1,
		SERVER_BANS              = 1 << 2,
		SERVER_EMOJIS            = 1 << 3,
		SERVER_INTEGRATIONS      = 1 << 4,
		SERVER_WEBHOOKS          = 1 << 5,
		SERVER_INVITES           = 1 << 6,
		SERVER_VOICE_STATES      = 1 << 7,
		SERVER_PRESENCES         = 1 << 8,
		SERVER_MESSAGES          = 1 << 9,
		SERVER_MESSAGE_REACTIONS = 1 << 10,
		SERVER_MESSAGE_TYPING    = 1 << 11,
		DIRECT_MESSAGES          = 1 << 12,
		DIRECT_MESSAGE_REACTIONS = 1 << 13,
		DIRECT_MESSAGE_TYPING    = 1 << 14,
	};

	class BaseDiscordClient : public GenericMessageReceiver {
	public:
		BaseDiscordClient() = default;
		BaseDiscordClient(const std::string _token) { start(_token); }
		~BaseDiscordClient();

		//important note, all requests on sync mode throw on an http error

		using RequestCallback = std::function<void(Response)>;
		Response request(const RequestMethod method, Route path, const std::string jsonParameters = "",
			const std::vector<Part>& multipartParameters = {},
			RequestCallback callback = nullptr, const RequestMode mode = Sync_AsyncQueue);
		struct Request {
			BaseDiscordClient& client;
			const RequestMethod method;
			const Route url;
			const std::string jsonParameters;
			const std::vector<Part> multipartParameters;
			const BaseDiscordClient::RequestCallback callback;
			RequestMode mode;
			inline void operator()() const {
				client.request(method, url, jsonParameters, multipartParameters, callback, mode);
			}
		};

		template<class ParmType>
		void requestAsync(const RequestMethod method, Route path, std::function<void(ParmType)> callback, const std::string jsonParameters = "",
			const std::initializer_list<Part>& multipartParameters = {}, const RequestMode mode = Async) {
			postTask(static_cast<PostableTask>(
				Request{ *this, method, path, jsonParameters, multipartParameters, callback ? RequestCallback([callback](Response r) {
					callback(static_cast<ParmType>(r));
				}) : RequestCallback(nullptr), mode }
			));
		}

		template<class ParmType>
		Response requestSync(const RequestMethod method, Route path, std::function<void(ParmType)> callback, const std::string jsonParameters = "",
			const std::initializer_list<Part>& multipartParameters = {}, const RequestMode mode = Sync) {
			return request(method, path, jsonParameters, multipartParameters, callback ? RequestCallback([callback](Response r) {
				callback(static_cast<ParmType>(r));
			}) : RequestCallback(nullptr), mode );
		}

		const Route path(const char* source, std::initializer_list<std::string> values = {});

#ifndef SLEEPY_DEFAULT_REQUEST_MODE
	#ifdef SLEEPY_DEFAULT_REQUEST_MODE_ASYNC
		#define SLEEPY_DEFAULT_REQUEST_MODE Async;
	#elif defined(SLEEPY_DEFAULT_REQUEST_MODE_SYNC)
		#define SLEEPY_DEFAULT_REQUEST_MODE Sync;
	#else
		#define SLEEPY_DEFAULT_REQUEST_MODE Sync_AsyncQueue;
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
			if (settings.mode & UseRequestAsync) {
				requestAsync<
					typename RequestSettingsClass::ParmType
				>(method, path, settings.callback, jsonParameters, multipartParameters, settings.mode);
			} else {
				if (settings.callback)
					//having an invalid callback here would cause bugs
					return requestSync<
						typename RequestSettingsClass::ParmType
					>(method, path, settings.callback, jsonParameters, multipartParameters, settings.mode);
				else
					return request(method, path, jsonParameters, multipartParameters, nullptr, settings.mode);
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
			const std::string jsonParameters = "", const std::initializer_list<Part>& multipartParameters = {}
			) {
			return RequestModeType<mode>::template doRequest<ParmType, Callback>(*this, method, path, jsonParameters, multipartParameters, callback);
		}

		void testFunction(std::string teststring);

		//channel functions
		ObjectResponse<Channel     > getChannel              (Snowflake<Channel> channelID                                                                                 , RequestSettings<ObjectResponse<Channel>> settings = {});
		ObjectResponse<Channel     > editChannel             (Snowflake<Channel> channelID, std::string name = "", std::string topic = ""                                  , RequestSettings<ObjectResponse<Channel>> settings = {});
		ObjectResponse<Channel     > editChannelName         (Snowflake<Channel> channelID, std::string name                                                               , RequestSettings<ObjectResponse<Channel>> settings = {});
		ObjectResponse<Channel     > editChannelTopic        (Snowflake<Channel> channelID, std::string topic                                                              , RequestSettings<ObjectResponse<Channel>> settings = {});
		ObjectResponse<Channel     > deleteChannel           (Snowflake<Channel> channelID                                                                                 , RequestSettings<ObjectResponse<Channel>> settings = {});
		enum GetMessagesKey {na, around, before, after, limit};
		ArrayResponse <Message     > getMessages             (Snowflake<Channel> channelID, GetMessagesKey when, Snowflake<Message> messageID, uint8_t limit = 0);
		ObjectResponse<Message     > getMessage              (Snowflake<Channel> channelID, Snowflake<Message> messageID                                                   , RequestSettings<ObjectResponse<Message>> settings = {});  //to do add more then one message return
		ObjectResponse<Message     > sendMessage             (Snowflake<Channel> channelID, std::string message, Embed embed = Embed::Flag::INVALID_EMBED, bool tts = false, RequestSettings<ObjectResponse<Message>> settings = {});
		ObjectResponse<Message     > sendMessage             (SendMessageParams params                                                                                     , RequestSettings<ObjectResponse<Message>> settings = {});
		ObjectResponse<Message     > uploadFile              (Snowflake<Channel> channelID, std::string fileLocation, std::string message                                  , RequestSettings<ObjectResponse<Message>> settings = {});
		BoolResponse                 addReaction             (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji                                , RequestSettings<BoolResponse           > settings = {});
		BoolResponse                 removeReaction          (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, Snowflake<User> userID = "@me");
		ArrayResponse <Reaction    > getReactions            (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji                                , RequestSettings<ArrayResponse<Reaction>> settings = {});
		StandardResponse             removeAllReactions      (Snowflake<Channel> channelID, Snowflake<Message> messageID                                                   , RequestSettings<StandardResponse       > settings = {});
		ObjectResponse<Message     > editMessage             (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string newMessage                           , RequestSettings<ObjectResponse<Message>> settings = {});
		BoolResponse                 deleteMessage           (Snowflake<Channel> channelID, Snowflake<Message> messageID                                                   , RequestSettings<BoolResponse           > settings = {});
		BoolResponse                 bulkDeleteMessages      (Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messageIDs                                     , RequestSettings<BoolResponse           > settings = {});
		/*allow is a bitwise value of all allowed permissions
		deny is a bitwise value of all deisallowed permissions
		type is "member" for a user or "role" for a role*/
		BoolResponse                 editChannelPermissions  (Snowflake<Channel> channelID, Snowflake<Overwrite> overwriteID, int allow, int deny, std::string type);           //to do test this
		ArrayResponse <Invite      > getChannelInvites       (Snowflake<Channel> channelID                                                                                 , RequestSettings<ArrayResponse<Invite  >> settings = {});
		ObjectResponse<Invite      > createChannelInvite     (Snowflake<Channel> channelID, const uint64_t maxAge = 0, const uint64_t maxUses = 0, const bool temporary = false, const bool unique = false);
		BoolResponse                 removeChannelPermission (Snowflake<Channel> channelID, std::string ID                                                                 , RequestSettings<BoolResponse           > settings = {});
		BoolResponse                 sendTyping              (Snowflake<Channel> channelID                                                                                 , RequestSettings<BoolResponse           > settings = {});
		ArrayResponse <Message     > getPinnedMessages       (Snowflake<Channel> channelID                                                                                 , RequestSettings<ArrayResponse<Message >> settings = {});
		BoolResponse                 pinMessage              (Snowflake<Channel> channelID, Snowflake<Message> messageID                                                   , RequestSettings<BoolResponse           > settings = {});
		BoolResponse                 unpinMessage            (Snowflake<Channel> channelID, Snowflake<Message> messageID                                                   , RequestSettings<BoolResponse           > settings = {});
		StandardResponse             addRecipient            (Snowflake<Channel> channelID, Snowflake<User> userID                                                         , RequestSettings<StandardResponse       > settings = {});
		StandardResponse             removeRecipient         (Snowflake<Channel> channelID, Snowflake<User> userID                                                         , RequestSettings<StandardResponse       > settings = {});
		//IntelliSense Help
		/*functions with more then one name to make life easy for users that use IntelliSense*/
		inline BoolResponse          deleteReaction          (Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji) { return removeReaction         (channelID, messageID, emoji); }
		inline StandardResponse      deleteAllReactions      (Snowflake<Channel> channelID, Snowflake<Message> messageID                   ) { return removeAllReactions     (channelID, messageID       ); }
		inline BoolResponse          deleteChannelPermission (Snowflake<Channel> channelID, std::string               ID                   ) { return removeChannelPermission(channelID,        ID       ); }
		inline StandardResponse      deleteRecipient         (Snowflake<Channel> channelID, Snowflake<User   >    _userID                  ) { return removeRecipient        (channelID,   _userID       ); }
		//For Convenience
		inline ObjectResponse<Message> editMessage(Message message, std::string newMessage) { return editMessage(message.channelID, message.ID, newMessage); }
		inline ObjectResponse<Message> sendMessage(Snowflake<Channel> channelID, std::string message, RequestSettings<ObjectResponse<Message>> settings) {
			return sendMessage(channelID, message, Embed::Flag::INVALID_EMBED, false, settings);
		}

		//server functions
		//ObjectResponse<Server      > createServer to do add this
		ObjectResponse<Server      > getServer               (Snowflake<Server> serverID                                                         , RequestSettings<ObjectResponse<Server      >> settings = {});
		//edit Server		//to do add this
		ObjectResponse<Server      > deleteServer            (Snowflake<Server> serverID                                                         , RequestSettings<ObjectResponse<Server      >> settings = {});
		ArrayResponse <Channel     > getServerChannels       (Snowflake<Server> serverID                                                         , RequestSettings<ArrayResponse<Channel      >> settings = {});
		ObjectResponse<Channel     > createTextChannel       (Snowflake<Server> serverID, std::string name                                       , RequestSettings<ObjectResponse<Channel     >> settings = {});
		ArrayResponse <Channel     > editChannelPositions    (Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions, RequestSettings<ArrayResponse<Channel      >> settings = {});
		ObjectResponse<ServerMember> getMember               (Snowflake<Server> serverID, Snowflake<User> userID                                 , RequestSettings<ObjectResponse<ServerMember>> settings = {});
		ArrayResponse <ServerMember> listMembers             (Snowflake<Server> serverID, uint16_t limit = 0, std::string after = ""             , RequestSettings<ArrayResponse<ServerMember >> settings = {});
		ObjectResponse<ServerMember> addMember               (Snowflake<Server> serverID, Snowflake<User> userID, std::string accesToken, std::string nick = "", std::vector<Role> roles = {}, bool mute = false, bool deaf = false); //to do test this
		BoolResponse                 editMember              (Snowflake<Server> serverID, Snowflake<User> userID, std::string nickname = "", std::vector<Snowflake<Role>> roles = {}, int8_t mute = -1, int8_t deaf = -1, Snowflake<Channel> channelID = {});
		BoolResponse                 muteServerMember        (Snowflake<Server> serverID, Snowflake<User> userID, bool mute = true               , RequestSettings<BoolResponse                 > settings = {});  //to do test this
		BoolResponse                 editNickname            (Snowflake<Server> serverID, std::string newNickname                                , RequestSettings<BoolResponse                 > settings = {});
		BoolResponse                 addRole                 (Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID         , RequestSettings<BoolResponse                 > settings = {});
		BoolResponse                 removeRole              (Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID         , RequestSettings<BoolResponse                 > settings = {});
		BoolResponse                 kickMember              (Snowflake<Server> serverID, Snowflake<User> userID                                 , RequestSettings<BoolResponse                 > settings = {});
		ArrayResponse <User        > getBans                 (Snowflake<Server> serverID                                                         , RequestSettings<ArrayResponse<User          >> settings = {});  //to do test this
		BoolResponse                 banMember               (Snowflake<Server> serverID, Snowflake<User> userID                                 , RequestSettings<BoolResponse                 > settings = {});
		BoolResponse                 unbanMember             (Snowflake<Server> serverID, Snowflake<User> userID                                 , RequestSettings<BoolResponse                 > settings = {});
		ArrayResponse <Role        > getRoles                (Snowflake<Server> serverID                                                         , RequestSettings<ArrayResponse<Role          >> settings = {});
		ObjectResponse<Role        > createRole              (Snowflake<Server> serverID, std::string name = "", Permission permissions = Permission::NONE, unsigned int color = 0, bool hoist = false, bool mentionable = false);
		ArrayResponse <Role        > editRolePosition        (Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions, RequestSettings<ArrayResponse<Role>> settings = {});  //to do test this
		StringResponse               editRole                (Snowflake<Server> serverID, Snowflake<Role> roleID, std::string name = "", Permission permissions = Permission::NONE, uint32_t color = 1 << 24, int8_t hoist = -1, int8_t mentionable = -1);
		BoolResponse                 deleteRole              (Snowflake<Server> serverID, Snowflake<Role> roleID                                 , RequestSettings<BoolResponse                 > settings = {});
		//get prune count	needs testing to know what object they are talking about
		StandardResponse             pruneMembers            (Snowflake<Server> serverID, const unsigned int numOfDays                           , RequestSettings<StandardResponse             > settings = {});  //to do test
		ArrayResponse <VoiceRegion > getVoiceRegions         (                                                                                     RequestSettings<ArrayResponse<VoiceRegion   >> settings = {});
		ArrayResponse <Invite      > getServerInvites        (Snowflake<Server> serverID                                                         , RequestSettings<ArrayResponse<Invite        >> settings = {});
		StringResponse               getIntegrations         (Snowflake<Server> serverID                                                         , RequestSettings<StringResponse               > settings = {});  //needs whatever a integration class is  //to do test
		BoolResponse                 createIntegration       (Snowflake<Server> serverID, std::string type, std::string integrationID            , RequestSettings<BoolResponse                 > settings = {});  //to do test
		BoolResponse                 editIntergration        (Snowflake<Server> serverID, std::string integrationID, int expireBegavior, int expireGracePeriod, bool enbleEmoticons); //to do test
		BoolResponse                 deleteIntegration       (Snowflake<Server> serverID, std::string integrationID                              , RequestSettings<BoolResponse                 > settings = {});  //to do test this
		BoolResponse                 syncIntegration         (Snowflake<Server> serverID, std::string integrationID                              , RequestSettings<BoolResponse                 > settings = {});  //to do test this
		ObjectResponse<ServerEmbed > getServerEmbed          (Snowflake<Server> serverID                                                         , RequestSettings<ObjectResponse<ServerEmbed  >> settings = {});
		//edit server embed   I don't know what the perms are

		//Invite functions
		ObjectResponse<Invite      > inviteEndpoint          (RequestMethod method, std::string inviteCode, RequestSettings<ObjectResponse<Invite>> settings = {});
		ObjectResponse<Invite      > getInvite               (std::string inviteCode                      , RequestSettings<ObjectResponse<Invite>> settings = {});  //to do test this
		ObjectResponse<Invite      > deleteInvite            (std::string inviteCode                      , RequestSettings<ObjectResponse<Invite>> settings = {});  //to do test this
		ObjectResponse<Invite      > acceptInvite            (std::string inviteCode                      , RequestSettings<ObjectResponse<Invite>> settings = {});	//not available to bot accounts  //to do test this

		//User functions
		ObjectResponse<User        > getCurrentUser          (                            RequestSettings<ObjectResponse<User     >> settings = {});
		ObjectResponse<User        > getUser                 (Snowflake<User> userID    , RequestSettings<ObjectResponse<User     >> settings = {});
		//User editCurrentUser();
		ArrayResponse <Server>     getServers                (                            RequestSettings<ArrayResponse<Server    >> settings = {});
		BoolResponse               leaveServer               (Snowflake<Server> serverID, RequestSettings<BoolResponse             > settings = {});
		ArrayResponse <Channel   > getDirectMessageChannels  (                            RequestSettings<ArrayResponse<Channel   >> settings = {});
		ObjectResponse<Channel   > createDirectMessageChannel(std::string recipientID   , RequestSettings<ObjectResponse<Channel  >> settings = {});
		//ObjectResponse<DMChannel > createGroupDirectMessageChannel(std:vector<std::string> accessTokens, ) to do add this
		ArrayResponse <Connection> getUserConnections        (                            RequestSettings<ArrayResponse<Connection>> settings = {});

		//Voice Functions
		//getVoiceRegions

		//Webhook functions
		ObjectResponse<Webhook> createWebhook                (Snowflake<Channel> channelID, std::string name, std::string avatar = "", RequestSettings<ObjectResponse<Webhook>> settings = {});  //to do test this
		ArrayResponse <Webhook> getChannelWebhooks           (Snowflake<Channel> channelID                                           , RequestSettings<ArrayResponse <Webhook>> settings = {});
		ArrayResponse <Webhook> getServerWebhooks            (Snowflake<Server> serverID                                             , RequestSettings<ArrayResponse <Webhook>> settings = {});
		ObjectResponse<Webhook> getWebhook                   (Snowflake<Webhook> webhookID, std::string webhookToken = ""            , RequestSettings<ObjectResponse<Webhook>> settings = {});  //to do test this
		ObjectResponse<Webhook> editWebhook                  (Snowflake<Webhook> webhookID, std::string webhookToken = "", std::string name = "", std::string avatar = "");    //you can leave token or name as null //to do test this
		BoolResponse            deleteWebhook                (Snowflake<Webhook> webhookID, std::string webhookToken = ""            , RequestSettings<BoolResponse           > settings = {});
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
		const bool hasIntents() { return intentsIsSet; }
		const IntentsRaw getIntents() { return intents; }
		void setIntents(IntentsRaw newIntents) { intentsIsSet = true; intents = static_cast<Intent>(newIntents); }
		void quit() { quit(false); }	//public function for diconnecting
		virtual void run();

		//array of intents
		template<class Container, typename T = typename Container::value_type>
		void setIntents(const Container& listOfIntents) {
			IntentsRaw target = 0;
			for (Intent intent : listOfIntents)
				target = target | static_cast<IntentsRaw>(intent);
			setIntents(target);
		}

		//parameter pack of intents
		template<typename... Types>
		void setIntents(Intent first, Intent second, Types... others) {
			std::initializer_list<Intent> intents = { first, second, others... };
			setIntents(intents);
		}

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
		void connectToVoiceChannel(VoiceContext& voiceContext, VoiceMode settings = normal);
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
		virtual void onExceededRateLimit(bool global, std::time_t timeTilRetry, Request request);

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
		virtual void onResumed           (                              );
		virtual void onDeleteServer      (UnavailableServer server      );
		virtual void onEditServer        (Server            server      );
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
		virtual void onDeleteChannel     (Channel            channel    );
		virtual void onEditChannel       (Channel            channel    );
		virtual void onPinMessage        (Snowflake<Channel> channelID, std::string lastPinTimestamp);
		virtual void onPresenceUpdate    (PresenceUpdate     presenseUpdate);
		virtual void onEditUser          (User               user       );
		virtual void onEditUserNote      (const json::Value& jsonMessage);
		virtual void onEditUserSettings  (const json::Value& jsonMessage);
		virtual void onEditVoiceState    (VoiceState&        state      );
		virtual void onTyping            (Snowflake<Channel> channelID, Snowflake<User> userID, time_t timestamp);
		virtual void onDeleteMessages    (Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messages);
		virtual void onEditMessage       (MessageRevisions   revisioins );
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
		void resetHeartbeatValues();
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
		virtual void stopClient() {}
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
		Intent intents;
		bool intentsIsSet = false;
		Snowflake<User> userID;
		void getTheGateway();
		std::string theGateway;
		bool ready = false;
		bool quiting = false;
		bool bot = true;
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
		RateLimiter rateLimiter;

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
