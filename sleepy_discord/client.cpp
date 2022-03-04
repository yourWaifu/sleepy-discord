#if _MSC_VER && !__INTEL_COMPILER
#pragma warning( disable: 4307 )  //ignore integer overflow, becuase we are taking advantage of it
#endif

#include <chrono>
#include <functional>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include "client.h"
#include "version_helper.h"
//#include "json.h"
#include "rapidjson/document.h"
#ifdef SLEEPY_USE_HARD_CODED_GATEWAY
	#include <cstring>
#endif

namespace SleepyDiscord {
	void BaseDiscordClient::start(const std::string _token, const char maxNumOfThreads, int _shardID, int _shardCount) {
		if (!scheduleHandler) {
			setError(CANT_SCHEDULE);
			return;
		}
		
		ready = false;
		quiting = false;
		bot = true;
		setToken(_token);
		if (_shardID != 0 || _shardCount != 0)
			setShardID(_shardID, _shardCount);

		messagesRemaining = 4;
	}

	BaseDiscordClient::~BaseDiscordClient() {
		ready = false;
		if (heart.isValid()) heart.stop();
		stopReconnecting();
	}

	Response BaseDiscordClient::request(const RequestMethod method, Route path, const std::string jsonParameters,
		const std::vector<Part>& multipartParameters, RequestCallback callback, const RequestMode mode
	) {
		//check if rate limited
		Response response;
		const time_t currentTime = getEpochTimeMillisecond();
		response.birth = currentTime;
		Route::Bucket bucket = path.bucket(method);

		bool shouldCallCallback = true;
		const auto handleCallbackCall = [&]() {
			if (shouldCallCallback && callback)
				callback(response);
		};
		const auto handleExceededRateLimit = [=, &shouldCallCallback](std::time_t timeTilRetry) {
			onExceededRateLimit(
				rateLimiter.isGlobalRateLimited, timeTilRetry,
				{ *this, method, path, jsonParameters, multipartParameters, callback, mode },
				shouldCallCallback
			);
		};

		time_t nextTry = rateLimiter.getLiftTime(bucket, currentTime);
		if (0 < nextTry) {
			handleExceededRateLimit(nextTry - currentTime);
			response.statusCode = TOO_MANY_REQUESTS;
			onError(TOO_MANY_REQUESTS,
				"Too many request going to " +
					std::string(getMethodName(method)) + " " +
					path.url());
			handleCallbackCall();
			return response;
		}
		{	//the { is used so that onResponse is called after session is removed to make debugging performance issues easier
			//request starts here
			Session session;
			session.setUrl("https://discord.com/api/v8/" + path.url());
			std::vector<HeaderPair> header = {
				{ "Authorization", bot ? "Bot " + getToken() : getToken() },
				{ "User-Agent", userAgent },
			};
			if (jsonParameters != "") {
				session.setBody(&jsonParameters);
				header.push_back({ "Content-Type"  , "application/json"                      });
				header.push_back({ "Content-Length", std::to_string(jsonParameters.length()) });
			} else if (0 < multipartParameters.size()) {
				session.setMultipart(multipartParameters);
				header.push_back({ "Content-Type", "multipart/form-data" });
			} else {
				header.push_back({ "Content-Length", "0" });
			}
			session.setHeader(header);

			//Do the response
			switch (method) {
			case Post: case Patch: case Delete: case Get: case Put:
				response = session.request(method);
				break;
			default: response.statusCode = BAD_REQUEST; break; //unexpected method
			}

			//rate limit check
			if (response.header["X-RateLimit-Remaining"] == "0" && response.statusCode != TOO_MANY_REQUESTS) {
				std::tm date = {};
				//for some reason std::get_time requires gcc 5
				std::istringstream dateStream(response.header["Date"]);
				dateStream >> std::get_time(&date, "%a, %d %b %Y %H:%M:%S GMT");
				const double resetTime = std::stod(response.header["X-RateLimit-Reset"]);
				const time_t reset = time_t(resetTime) + 1; //add one second for lost precision
				const std::string& xBucket = response.header["X-RateLimit-Bucket"];
#if defined(_WIN32) || defined(_WIN64)
				std::tm gmTM;
				std::tm* const resetGM = &gmTM;
				gmtime_s(resetGM, &reset);
#elif defined(__STDC_LIB_EXT1__)
				std::tm gmTM;
				std::tm* resetGM = &gmTM;
				gmtime_s(&reset, resetGM);
#else
				std::tm* resetGM = std::gmtime(&reset);
#endif
				const time_t resetDelta = (std::mktime(resetGM) - std::mktime(&date)) * 1000;
				rateLimiter.limitBucket(bucket, xBucket, resetDelta + getEpochTimeMillisecond());
				onDepletedRequestSupply(bucket, resetDelta);
			}

			//status checking
			switch (response.statusCode) {
			case OK: case CREATED: case NO_CONTENT: case NOT_MODIFIED: break;
			case TOO_MANY_REQUESTS:
				{   //this should fall down to default
					std::string rawRetryAfter = response.header["Retry-After"];
					//the 5 is an arbitrary number, and there's 1000 ms in a second
					int retryAfter = rawRetryAfter != "" ? std::stoi(rawRetryAfter) : 5;
					retryAfter *= 1000; //convert to milliseconds
					rateLimiter.isGlobalRateLimited = response.header.find("X-RateLimit-Global") != response.header.end();
					rateLimiter.nextRetry = getEpochTimeMillisecond() + retryAfter;
					const std::string& xBucket = response.header["X-RateLimit-Bucket"];
					if (!rateLimiter.isGlobalRateLimited) {
						rateLimiter.limitBucket(bucket, xBucket, rateLimiter.nextRetry);
						onDepletedRequestSupply(bucket, retryAfter);
					}
					handleExceededRateLimit(retryAfter);
				}
			default:
				{		//error
					const ErrorCode code = static_cast<ErrorCode>(response.statusCode);
					setError(code);		//https error
					if (!response.text.empty()) {
						//json::Values values = json::getValues(response.text.c_str(),
						//{ "code", "message" });	//parse json to get code and message
						rapidjson::Document document;
						document.Parse(response.text.c_str());
						if (!document.IsObject()) {
							onError(GENERAL_ERROR, "No error code or message from Discord");
							break;
						}

						auto errorCode = document.FindMember("code");
						auto errorMessage = document.FindMember("message");
						if (errorCode != static_cast<rapidjson::GenericValue<rapidjson::UTF8<>>::ConstMemberIterator>(document.MemberEnd())){
							std::size_t fullErrorMessageSize = 0;
							fullErrorMessageSize += path.getPath().length();
							fullErrorMessageSize += 1;
							fullErrorMessageSize += response.text.length();
							fullErrorMessageSize += 1;
							std::string message = (
									errorMessage != static_cast<rapidjson::GenericValue<rapidjson::UTF8<>>::ConstMemberIterator>(document.MemberEnd())
									?
										errorMessage->value.GetString()
									:
										""
							);
							fullErrorMessageSize += message.length();
							std::string fullErrorMessage;
							fullErrorMessage.reserve(fullErrorMessageSize);
							fullErrorMessage += path.getPath();
							fullErrorMessage += '\n';
							fullErrorMessage += response.text;
							fullErrorMessage += '\n';
							fullErrorMessage += message;

							onError(
								static_cast<ErrorCode>(errorCode->value.GetInt()),
								std::move(fullErrorMessage)
							);
						} else if (!response.text.empty()) {
							onError(ERROR_NOTE, response.text);
						}
					}
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
							if (static_cast<int>(mode) & static_cast<int>(ThrowError))
								throw code;
#endif
				} break;
			}

			handleCallbackCall();
		}
		onResponse(response);
		return response;
	}

	const Route BaseDiscordClient::path(const char * source, std::initializer_list<std::string> values) {
		return Route(source, values);
	}

	std::shared_ptr<ServerCache> BaseDiscordClient::createServerCache() {
		setServerCache(std::make_shared<ServerCache>());
		return getServerCache();
	}

	void BaseDiscordClient::setServerCache(std::shared_ptr<ServerCache> cache) {
		serverCache = cache;
		if ((ready || !isBot()) && serverCache->size() == 0)
			*serverCache = getServers().get<Cache>();
	}

	void BaseDiscordClient::onDepletedRequestSupply(const Route::Bucket&, time_t) {
	}

	void BaseDiscordClient::onExceededRateLimit(bool, std::time_t timeTilRetry, Request request, bool& continueRequest) {
		bool shouldScheduleNewRequest =
			static_cast<int>(request.mode) & static_cast<int>(AsyncQueue);
		continueRequest = !shouldScheduleNewRequest;
		if (shouldScheduleNewRequest) {
			//since we are scheduling the request, I think we should make it async
			request.mode = Async;
			schedule(request, timeTilRetry);
		}
	}

	void BaseDiscordClient::updateStatus(std::string gameName, uint64_t idleSince, Status status, bool afk) {
		std::string statusString[] = {
			"", "online", "dnd", "idle", "invisible", "offline"
		};

		sendL(json::createJSON({
			{ "op", json::integer(STATUS_UPDATE) },
			{ "d", json::createJSON({
				{"since", idleSince != 0 ? json::UInteger(idleSince) : "null"},
				{"game", gameName != "" ? json::createJSON({
					{"name", json::string(gameName)},
					{"type", json::integer(0)}
				}) : "null"},
				{ "status", SleepyDiscord::json::string(statusString[status]) },
				{ "afk", SleepyDiscord::json::boolean(afk) }
			})}
		}));
	}

	void BaseDiscordClient::requestServerMembers(ServerMembersRequest request) {
		auto data = json::toJSON(request);
		std::string stringData = json::stringify(data);

		std::string query;
		query.reserve(14 + stringData.length());
		query += "{\"op\":8,\"d\":";
		query += stringData;
		query += "}";

		sendL(query);
	}

	void BaseDiscordClient::waitTilReady() {
		while (!ready) sleep(1000);
	}

	void BaseDiscordClient::setShardID(int _shardID, int _shardCount) {
		shardID = _shardID;
		shardCount = _shardCount;
	}

	void BaseDiscordClient::getTheGateway() {
#ifdef SLEEPY_USE_HARD_CODED_GATEWAY
	#ifndef SLEEPY_HARD_CODED_GATEWAY
		#define SLEEPY_HARD_CODED_GATEWAY "wss://gateway.discord.gg/?v=8"
	#endif
		theGateway = SLEEPY_HARD_CODED_GATEWAY;	//This is needed for when session is disabled
#else
		Session session;
		session.setUrl("https://discord.com/api/gateway");
		Response a = session.request(Get);	//todo change this back to a post
		if (!a.text.length()) {	//error check
			quit(false, true);
			return setError(GATEWAY_FAILED);
		}
		if (!theGateway.empty())
			theGateway.clear();
		//getting the gateway
		for (unsigned int position = 0, j = 0; ; ++position) {
			if (a.text[position] == '"')
				++j;
			else if (j == 3) {
				const unsigned int start = position;
				while (a.text[++position] != '"');
				unsigned int size = position - start;
				theGateway.reserve(32);
				theGateway.append(a.text, start, size);
				theGateway += "/?v=8";
				break;
			}
		}
#endif
		if (useTrasportConnection == 1)
			theGateway += "&compress=zlib-stream";
	}

	void BaseDiscordClient::sendIdentity() {
		//{
		//	"op":2,
		//	 "d":{
		//		"token":my_token,
		//		"properties":{
		//			$os":"windows 10",
		//			"$browser":"Sleepy_Discord",
		//			"$device":"Sleepy_Discord",
		//		},
		//		"compress":false,
		//		"large_threshold":250			/I don't know what this does
		//	}
		//}
#if defined(_WIN32) || defined(_WIN64)
		const char * os = "Windows";
#elif defined(__APPLE__) || defined(__MACH__)
		const char * os = "macOS";
#elif defined(__linux__) || defined(linux) || defined(__linux)
		const char * os = "Linux";
#elif defined __FreeBSD__
		const char * os = "FreeBSD";
#elif defined(unix) || defined(__unix__) || defined(__unix)
		const char * os = "Unix";
#else
		const char* os = "\\u00AF\\\\_(\\u30C4)_\\/\\u00AF";  //shrug I dunno
#endif
		std::string identity;
		identity.reserve(272); //remember to change this number when editing identity
		identity += 
		"{"
			"\"op\":2,"
			"\"d\":{"
				"\"token\":\""; identity += getToken(); identity += "\","
				"\"properties\":{"
					"\"$os\":\""; identity += os; identity += "\","
					"\"$browser\":\"Sleepy_Discord\","
					"\"$device\":\"Sleepy_Discord\""
				"},"
				"\"compress\":";
					identity += compressionHandler && useTrasportConnection != 1 ?
						"true" : "false"; identity += ",";
		if (shardCount != 0 && shardID <= shardCount) {
			identity +=
				"\"shard\":[";
			identity +=	
					std::to_string(shardID); identity += ",";
			identity +=
					std::to_string(shardCount);
			identity +=
				"],";
		}
		if (hasIntents()) {
			identity +=
				"\"intents\":";
			identity +=
				std::to_string(intents);
		identity +=
				',';
		}
		identity +=
				"\"large_threshold\":250"
			"}"
		"}";
		sendL(identity);
	}

	void BaseDiscordClient::sendResume() {
		std::string resume;
		resume.reserve(208);
		resume +=
		"{"
			"\"op\":6,"
			"\"d\":{"
				"\"token\":\""; resume += getToken(); resume += "\","
				"\"session_id\":\""; resume += sessionID; resume += "\","
				"\"seq\":"; resume += std::to_string(lastSReceived); resume +=
			"}"
		"}";
		sendL(resume);
		onResume();
	}

	void BaseDiscordClient::quit(bool isRestarting, bool isDisconnected) {
		if (!isRestarting)
			quiting = true;

#ifdef SLEEPY_VOICE_ENABLED
		//quit all voice connections
		for (VoiceConnection& voiceConnection : voiceConnections)
			voiceConnection.disconnect();
#endif
		if (heart.isValid()) heart.stop(); //stop heartbeating
		stopReconnecting();
		if (!isDisconnected) disconnectWebsocket(1000);
		stopClient();
		if (quiting) onQuit();
	}

	void BaseDiscordClient::restart() {
		quit(true);
		connect(theGateway, this, connection);
		onRestart();
	}

	void BaseDiscordClient::reconnect(const unsigned int status) {
		std::lock_guard<std::mutex> lock(connectionMutex);
		if (isCurrentlyWaitingToReconnect == true)
			return; //don't try to reconnect when waiting to reconnect
		isCurrentlyWaitingToReconnect = true;

		//before disconnecting, heartbeats need to stop or it'll crash
		//and if it doesn't, it'll cause another reconnect
		if (heart.isValid()) heart.stop();
		//reset some heartbeat values, done so we don't spam discord
		wasHeartbeatAcked = true;
		lastHeartbeat = 0;
		heartbeatInterval = 0;

		disconnectWebsocket(status);
		//it's recomemeded to get the gateway again after many reconnection tries
		// but the chance of the gateway being different is very low so this line is
		// commmented out as it seems to cause issues
		//if (consecutiveReconnectsCount % 16 == 10 || theGateway.empty()) getTheGateway();
		if (reconnectTimer.isValid())
			reconnectTimer.stop();
		reconnectTimer = schedule([this]() {
			std::lock_guard<std::mutex> lock(connectionMutex);
			isCurrentlyWaitingToReconnect = false;
			connect(theGateway, this, connection);
		}, getRetryDelay());
		consecutiveReconnectsCount += 1;

		if (useTrasportConnection == 1)
			compressionHandler->resetStream();
	}

	void BaseDiscordClient::stopReconnecting() {
		std::lock_guard<std::mutex> lock(connectionMutex);
		isCurrentlyWaitingToReconnect = false;
		consecutiveReconnectsCount = 0;
		if (reconnectTimer.isValid())
			reconnectTimer.stop();
	}

	void BaseDiscordClient::disconnectWebsocket(unsigned int code, const std::string reason) {
		disconnect(code, reason, connection);
		onDisconnect();
	}

	bool BaseDiscordClient::sendL(std::string message) {
		if (nextHalfMin <= getEpochTimeMillisecond()) {
			const unsigned int maxMessagesPerMin = 116;
			const unsigned int halfMinMilliseconds = 30000;
			const unsigned int maxMessagesPerHalfMin = maxMessagesPerMin / 2;

			nextHalfMin += halfMinMilliseconds;
			messagesRemaining = maxMessagesPerHalfMin;
		}

		if (--messagesRemaining < 0) {
			messagesRemaining = 0;
			setError(RATE_LIMITED);
			return false;
		}
		send(message, connection);
		return true;
	}

	constexpr unsigned int hash(const char* key, unsigned int i = 0) {
		return !key[i] ? 0 : (hash(key, i + 1) * 31) + key[i] - 'A';
	}

	void BaseDiscordClient::processMessage(const std::string &message) {
		auto docPtr = std::make_shared<rapidjson::Document>();
		rapidjson::Document& document = *docPtr;
		document.Parse(message.c_str(), message.length());
		//	{ "op", "d", "s", "t" }
		int op = document["op"].GetInt();
		json::Value& d = document["d"];
		switch (op) {
		case DISPATCH:
			lastSReceived = document["s"].GetInt();
			postTask(
				[this, docPtr, &d]() {
					rapidjson::Document& document = *docPtr;
					const json::Value& t = document["t"];
					handleDispatchEvent(t, d);
				}
			);
		break;
		case HELLO:
			heartbeatInterval = d["heartbeat_interval"].GetInt();
			heartbeat();
			if (!ready) sendIdentity();
			else sendResume();
			break;
		case RECONNECT:
			reconnect();
			break;
		case INVALID_SESSION:
			if (json::toBool(d) == true) {
				schedule(&BaseDiscordClient::sendResume, 2500);
			} else {
				sessionID = "";
				schedule(&BaseDiscordClient::sendIdentity, 2500);
			}
			break;
		case HEARTBEAT_ACK:
			wasHeartbeatAcked = true;
			onHeartbeatAck();
			break;
		}
	}

	void BaseDiscordClient::handleDispatchEvent(const json::Value& t, json::Value& d) {
		switch (hash(json::toStdString(t).c_str())) {
		case hash("READY"): {
			Ready readyData = d;
			sessionID = readyData.sessionID;
			bot = readyData.user.bot;
			userID = readyData.user;
			onReady(readyData);
			ready = true;
			stopReconnecting(); //Successfully connected
		} break;
		case hash("RESUMED"):
			stopReconnecting(); //Successfully connected
			onResumed();
			break;
		case hash("GUILD_CREATE"): {
			Server server(d);
			if (serverCache)
				serverCache->insert(server);
			onServer(server);
		} break;
		case hash("GUILD_DELETE"): {
			UnavailableServer server(d);
			if (serverCache) {
				findServerInCache(server.ID, [=](ServerCache::iterator& found) {
					serverCache->erase(found);
					});
			}
			onDeleteServer(server);
		} break;
		case hash("GUILD_UPDATE"): {
			Server server(d);
			accessServerFromCache(server.ID, [server](Server& foundServer) {
				json::mergeObj(foundServer, server);
				});
			onEditServer(server);
		} break;
		case hash("GUILD_BAN_ADD"): onBan(d["guild_id"], d["user"]); break;
		case hash("GUILD_BAN_REMOVE"): onUnban(d["guild_id"], d["user"]); break;
		case hash("GUILD_INTEGRATIONS_UPDATE"):                          break; //to do add this
		case hash("GUILD_MEMBER_ADD"): {
			Snowflake<Server> serverID = d["guild_id"];
			ServerMember member(d);
			appendObjectToCache(serverID, &Server::members, member);
			onMember(serverID, member);
		} break;
		case hash("GUILD_MEMBER_REMOVE"): {
			Snowflake<Server> serverID = d["guild_id"];
			User user = d["user"];
			eraseObjectFromCache(serverID, &Server::members, user.ID);
			onRemoveMember(serverID, user);
		} break;
		case hash("GUILD_MEMBER_UPDATE"): {
			Snowflake<Server> serverID = d["guild_id"];
			User user = d["user"];
			std::vector<Snowflake<Role>> roles = json::toArray<Snowflake<Role>>(d["roles"]);
			auto nickValue = d.FindMember("nick");
			std::string nick = nickValue != d.MemberEnd() && nickValue->value.IsString() ?
				json::toStdString(nickValue->value) : "";
			accessObjectFromCache(serverID, &Server::members, user.ID,
				[user, roles, nick](Server&, ServerMember& member) {
					member.user = user;
					member.roles = roles;
					member.nick = nick;
				}
			);
			onEditMember(serverID, user, roles, nick);
		} break;
		case hash("GUILD_MEMBERS_CHUNK"): onMemberChunk(d); break;
		case hash("GUILD_ROLE_CREATE"): {
			Snowflake<Server> serverID = d["guild_id"];
			Role role = d["role"];
			appendObjectToCache(serverID, &Server::roles, role);
			onRole(serverID, role);
		} break;
		case hash("GUILD_ROLE_UPDATE"):
		{
			Snowflake<Server> serverID = d["guild_id"];
			Role role = d["role"];
			accessObjectFromCache(serverID, &Server::roles, role.ID,
				[role](Server&, Role& foundRole) {
					foundRole = role;
				}
			);
			onEditRole(serverID, role);
		} break;
		case hash("GUILD_ROLE_DELETE"): {
			Snowflake<Server> serverID = d["guild_id"];
			Snowflake<Role> roleID = d["role_id"];
			eraseObjectFromCache(serverID, &Server::roles, roleID);
			onDeleteRole(serverID, roleID);
		} break;
		case hash("GUILD_EMOJIS_UPDATE"): onEditEmojis(d["guild_id"], json::toArray<Emoji>(d["emojis"])); break;
		case hash("CHANNEL_CREATE"): {
			Channel channel = d;
			appendObjectToCache(channel.serverID, &Server::channels, channel);
			onChannel(d);
		} break;
		case hash("CHANNEL_UPDATE"): {
			Channel channel = d;
			accessObjectFromCache(channel.serverID, &Server::channels, channel.ID,
				[channel](Server&, Channel& foundChannel) {
					foundChannel = channel;
				}
			);
			onEditChannel(d);
		} break;
		case hash("CHANNEL_DELETE"): {
			Channel channel = d;
			eraseObjectFromCache(channel.serverID, &Server::channels, channel.ID);
			onDeleteChannel(d);
		} break;
		case hash("CHANNEL_PINS_UPDATE"): {
			const json::Value& lastPinTimeValue = d["last_pin_timestamp"];
			onPinMessage(
				d["channel_id"],
				lastPinTimeValue.IsString() ?
				json::toStdString(d["last_pin_timestamp"]) : ""
			);
		} break;
		case hash("PRESENCE_UPDATE"): onPresenceUpdate(d); break;
		case hash("PRESENCES_REPLACE"):                          break;
		case hash("USER_UPDATE"): onEditUser(d); break;
		case hash("USER_SETTINGS_UPDATE"): onEditUserSettings(d); break;
		case hash("VOICE_STATE_UPDATE"): {
			VoiceState state(d);
#ifdef SLEEPY_VOICE_ENABLED
			if (!waitingVoiceContexts.empty()) {
				auto iterator = find_if(waitingVoiceContexts.begin(), waitingVoiceContexts.end(),
					[&state](const VoiceContext* w) {
						return state.channelID == w->channelID && w->sessionID == "";
					});
				if (iterator != waitingVoiceContexts.end()) {
					VoiceContext& context = **iterator;
					context.sessionID = state.sessionID;
					connectToVoiceIfReady(context);
				}
			}
#endif
			onEditVoiceState(state);
		} break;
		case hash("TYPING_START"): onTyping(d["channel_id"], d["user_id"], d["timestamp"].GetInt64() * 1000); break;
		case hash("MESSAGE_CREATE"): onMessage(d); break;
		case hash("MESSAGE_UPDATE"): onEditMessage(d); break;
		case hash("MESSAGE_DELETE"): onDeleteMessages(d["channel_id"], { d["id"] }); break;
		case hash("MESSAGE_DELETE_BULK"): onDeleteMessages(d["channel_id"], json::toArray<Snowflake<Message>>(d["ids"])); break;
		case hash("VOICE_SERVER_UPDATE"): {
			VoiceServerUpdate voiceServer(d);
#ifdef SLEEPY_VOICE_ENABLED
			if (!waitingVoiceContexts.empty()) {
				auto iterator = find_if(waitingVoiceContexts.begin(), waitingVoiceContexts.end(),
					[&voiceServer](const VoiceContext* w) {
						return voiceServer.serverID == w->serverID && w->endpoint == "";
					});
				if (iterator != waitingVoiceContexts.end()) {
					VoiceContext& context = **iterator;
					context.token = voiceServer.token;
					context.endpoint = voiceServer.endpoint;
					connectToVoiceIfReady(context);
				}
			}
#endif
			onEditVoiceServer(voiceServer);
		} break;
		case hash("MESSAGE_REACTION_ADD"): onReaction(d["user_id"], d["channel_id"], d["message_id"], d["emoji"]); break;
		case hash("MESSAGE_REACTION_REMOVE"): onDeleteReaction(d["user_id"], d["channel_id"], d["message_id"], d["emoji"]); break;
		case hash("MESSAGE_REACTION_REMOVE_ALL"): onDeleteAllReaction(d["guild_id"], d["channel_id"], d["message_id"]); break;
		case hash("APPLICATION_COMMAND_CREATE"): onAppCommand(d); break;
		case hash("APPLICATION_COMMAND_UPDATE"): onEditAppCommand(d); break;
		case hash("APPLICATION_COMMAND_DELETE"): onDeleteAppCommand(d); break;
		case hash("INTERACTION_CREATE"): onInteraction(d); break;
		case hash("STAGE_INSTANCE_CREATE"): onStageInstance(d); break;
		case hash("STAGE_INSTANCE_UPDATE"): onEditStageInstance(d); break;
		case hash("STAGE_INSTANCE_DELETE"): onDeleteStageInstance(d); break;
		default:
			onUnknownEvent(json::toStdString(t), d);
			break;
		}
		onDispatch(t, d);
	}

	void BaseDiscordClient::processMessage(const WebSocketMessage message) {
		switch (message.opCode) {
		case WebSocketMessage::OPCode::binary: {
			if (!compressionHandler)
				break;
			compressionHandler->uncompress(message.payload);
			
			//when using transport connections, Discord ends streams the flush siginal
			constexpr std::array<const char, 4> flushSiginal = { 0, 0, '\xFF', '\xFF'};
			constexpr std::size_t siginalLength = flushSiginal.max_size();
			bool endsWithFlushSiginal = false;
			if (useTrasportConnection == 1 && siginalLength <= message.payload.length()) {
				const auto compare = message.payload.compare(
					message.payload.length() - siginalLength, siginalLength,
					flushSiginal.data(), siginalLength);
				endsWithFlushSiginal = compare == 0;
			}

			//trasportConnection doesn't stop the stream
			bool streamEnded = useTrasportConnection != 1 && compressionHandler->streamEnded();

			if (streamEnded || endsWithFlushSiginal) {
				std::shared_ptr<std::string> uncompressed = std::make_shared<std::string>();
				compressionHandler->getOutput(*uncompressed);
				processMessage(*uncompressed);
			}
			break;
		}
		case WebSocketMessage::OPCode::text: {
			processMessage(message.payload);
			break;
		}
		default: break;
		}
	}

	void BaseDiscordClient::processCloseCode(const int16_t code) {
		setError(code);

		switch (code) {
		//Just reconnect
		case 1006:
		case UNKNOWN_ERROR:
		case UNKNOWN_OPCODE:
		case DECODE_ERROR:
		case NOT_AUTHENTICATED:
		case ALREADY_AUTHENTICATED:
		case INVALID_SEQ:
		case RATE_LIMITED:
		case SESSION_TIMEOUT:
		default:
			break;

		case 1000:
			if (!isQuiting())
				break;
			//else fall through

		//Might be Unrecoveralbe
		//We may need to stop to prevent a restart loop.
		case AUTHENTICATION_FAILED:
		case INVALID_SHARD:
		case SHARDING_REQUIRED:
		case INVALID_INTENTS:
		case DISALLOWED_INTENTS:
			return quit(false, true);
			break;

		case 4900: //Sleepy Discord reconnect
			//don't do another reconnect during a reconnect
			return;
		}
		reconnect();
	}

	void BaseDiscordClient::heartbeat() {
		if (heartbeatInterval <= 0 || isQuiting()) return; //sanity test
		
		//if time and timer are out of sync, trust time
		time_t currentTime = getEpochTimeMillisecond();
		time_t nextHeartbest;
		if (currentTime < (nextHeartbest = lastHeartbeat + heartbeatInterval)) {
			heart = schedule(&BaseDiscordClient::heartbeat, nextHeartbest - currentTime);
			return;
		}

		if (!wasHeartbeatAcked) {
			//dead connection
			reconnect();
			return; //don't heartbeat
		}

		sendHeartbeat();
		lastHeartbeat = currentTime;

		if (heart.isValid())
			heart.stop();
		heart = schedule(&BaseDiscordClient::heartbeat, heartbeatInterval);
	}

	//
	//Voice
	//

#ifdef SLEEPY_VOICE_ENABLED

	VoiceContext& BaseDiscordClient::createVoiceContext(Snowflake<Server> server, Snowflake<Channel> channel, BaseVoiceEventHandler * eventHandler) {
		Snowflake<Server> serverTarget = server != "" ? server : getChannel(channel).cast().serverID;
		voiceContexts.push_front({ serverTarget, channel, eventHandler });
		waitingVoiceContexts.emplace_front(&voiceContexts.front());
		return voiceContexts.front();
	}

	void BaseDiscordClient::connectToVoiceChannel(VoiceContext& voiceContext, VoiceMode settings) {
		std::string voiceState;
		/*The number 131 came from the number of letters in this string:
		  {"op": 4,"d" : {"guild_id": "18446744073709551615",
		  "channel_id" : "18446744073709551615","self_mute" : false,"self_deaf" : false}}
		  plus one
		*/
		voiceState.reserve(131);  //remember to update this when making changes to voice status
		voiceState +=
			"{"
				"\"op\": 4,"
				"\"d\": {"
					"\"guild_id\": \""; voiceState += voiceContext.serverID; voiceState += "\","
					"\"channel_id\""": \""; voiceState += voiceContext.channelID; voiceState += "\","
					"\"self_mute\"" ": "; voiceState += settings & mute   ? "true" : "false"; voiceState += ","
					"\"self_deaf\"" ": "; voiceState += settings & deafen ? "true" : "false"; voiceState +=
				"}"
			"}";
		sendL(voiceState);
		/*Discord will response by sending a VOICE_STATE_UPDATE and a
		  VOICE_SERVER_UPDATE payload. Take a look at processMessage
		  function at case VOICE_STATE_UPDATE and voiceServerUpdate
		  */
	}

	VoiceContext& BaseDiscordClient::connectToVoiceChannel(Snowflake<Server> server, Snowflake<Channel> channel, VoiceMode settings) {
		VoiceContext& target = createVoiceContext(server, channel);
		connectToVoiceChannel(target, settings);
		return target;
	}

	void BaseDiscordClient::connectToVoiceIfReady(VoiceContext& context) {
		if (context.endpoint == "" || context.sessionID == "") //check that we are ready
			return;

		//remove the port numbers at the end of the endpoint string
		std::string& givenEndpoint = context.endpoint;
		givenEndpoint = givenEndpoint.substr(0, givenEndpoint.find(':'));

		std::string endpoint = VoiceConnection::getWebSocketURI(givenEndpoint);

		//Add a new connection to the list of connections
		voiceConnections.emplace_front( this, context );
		VoiceConnection& voiceConnection = voiceConnections.front();

		connect(endpoint, &voiceConnection, voiceConnection.connection);

		//remove from wait list
		waitingVoiceContexts.remove_if(
			[&context](VoiceContext* right) {
				return &context == right;
			}
		);
	}

	void BaseDiscordClient::removeVoiceConnectionAndContext(VoiceConnection & connection) {
		const VoiceContext& context = connection.getContext();
		voiceConnections.remove_if(
			[&connection](VoiceConnection& right) {
				return connection == right;
			}
		);
		voiceContexts.remove_if(
			[&context](VoiceContext& right) {
				return &context == &right;
			}
		);
	}

#endif

	const time_t BaseDiscordClient::getEpochTimeMillisecond() {
		auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		return ms.time_since_epoch().count();
	}

	const std::string BaseDiscordClient::getEditPositionString(const std::vector<std::pair<std::string, uint64_t>>& positions) {
		std::vector<std::string> params(positions.size());
		for (auto& value : positions) {
			params.push_back(json::createJSON({
				{ "id", json::string(value.first) },
				{ "position", json::UInteger(value.second) }
			}));
		}
		return json::createJSONArray(params);
	}

	Route::Route(const std::string route, const std::initializer_list<std::string>& _values)
		: path(route), values(_values)
	{
		size_t targetSize = path.length();
		for (std::string replacement : values)
			targetSize += replacement.length();
		_url.reserve(targetSize);

		//In the future, use string view

		size_t offset = 0;
		for (std::string replacement : values) {
			const size_t start = path.find('{', offset);
			const size_t end = path.find('}', start);

			//the +1 and -1 removes the { and }
			const std::string identifier = path.substr(start + 1, end - start - 1);

			auto foundParam = majorParameters.find(identifier);
			if (foundParam != majorParameters.end()) {
				foundParam->second = replacement;
			}

			_url += path.substr(offset, start - offset);
			_url += replacement;
			offset = end + 1; //the +1 removes the }
		}
		_url += path.substr(offset, path.length() - offset);

	}

	Route::Route(const char* route) : Route(route, {}) {}

	const std::string Route::bucket(RequestMethod method) {
		std::string target;
		std::string methodString = std::to_string(method);
		size_t targetLength = methodString.length();
		for (auto param : majorParameters) {
			targetLength += param.second.length();
		}
		targetLength = path.length();
		target.reserve(targetLength);
		target += methodString;
		for (auto param : majorParameters) {
			target += param.second;
		}
		target += path;
		return target;
	}
}
