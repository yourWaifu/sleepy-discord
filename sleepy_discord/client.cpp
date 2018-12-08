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
		token = std::unique_ptr<std::string>(new std::string(_token)); //add client to list
		if (_shardID != 0 || _shardCount != 0)
			setShardID(_shardID, _shardCount);

		messagesRemaining = 4;
		getTheGateway();
		connect(theGateway, this, connection);
#ifndef SLEEPY_ONE_THREAD
		if (USE_RUN_THREAD <= maxNumOfThreads) runAsync();
#endif
	}

	BaseDiscordClient::BaseDiscordClient() : shardID(0), shardCount(0), ready(false), quiting(false), bot(true), messagesRemaining(0) {}

	BaseDiscordClient::~BaseDiscordClient() {
		ready = false;
		if (heart.isValid()) heart.stop();
	}

	Response BaseDiscordClient::request(const RequestMethod method, Route path, const std::string jsonParameters/*,
		cpr::Parameters httpParameters*/, const std::initializer_list<Part>& multipartParameters) {
		//check if rate limited
		Response response;
		const time_t currentTime = getEpochTimeMillisecond();
		if (isGlobalRateLimited) {
			if (nextRetry <= currentTime) {
				isGlobalRateLimited = false;
			} else {
				onExceededRateLimit(isGlobalRateLimited, nextRetry - currentTime, { *this, method, path, jsonParameters, multipartParameters });
				response.statusCode = TOO_MANY_REQUESTS;
				setError(response.statusCode);
				return response;
			}
		}
		const std::string bucket = path.bucket(method);
		auto bucketResetTimestamp = buckets.find(bucket);
		if (bucketResetTimestamp != buckets.end()) {
			if (bucketResetTimestamp->second <= currentTime) {
				buckets.erase(bucketResetTimestamp);
			} else {
				onExceededRateLimit(false, bucketResetTimestamp->second - currentTime, { *this, method, path, jsonParameters, multipartParameters });
				response.statusCode = TOO_MANY_REQUESTS;
				setError(response.statusCode);
				return response;
			}
		}
		{	//the { is used so that onResponse is called after session is removed to make debugging performance issues easier
			//request starts here
			Session session;
			session.setUrl("https://discordapp.com/api/v6/" + path.url());
			std::vector<HeaderPair> header = {
				{ "Authorization", bot ? "Bot " + getToken() : getToken() },
				{ "User-Agent", "DiscordBot (https://github.com/yourWaifu/SleepyDiscord, vtheBestVersion)" },
			};
			if (jsonParameters != "") {
				session.setBody(&jsonParameters);
				header.push_back({ "Content-Type"  , "application/json"                      });
				header.push_back({ "Content-Length", std::to_string(jsonParameters.length()) });
			//} else if (httpParameters.content != "") {	//this is broken for now
			//	session.SetParameters(httpParameters);
			} else if (0 < multipartParameters.size()) {
				session.setMultipart(multipartParameters);
				header.push_back({ "Content-Type", "multipart/form-data" });
			} else {
				header.push_back({ "Content-Length", "0" });
			}
			session.setHeader(header);
			//Response response;
			switch (method) {
			case Post: case Patch: case Delete: case Get: case Put:
				response = session.request(method);
				break;
			default: response.statusCode = BAD_REQUEST; break; //unexpected method
			}
			//status checking
			switch (response.statusCode) {
			case OK: case CREATED: case NO_CONTENT: case NOT_MODIFIED: break;
			case TOO_MANY_REQUESTS: {   //this should fall down to default
				int retryAfter = std::stoi(response.header["Retry-After"]);
				isGlobalRateLimited = response.header["X-RateLimit-Global"] == "true";
				nextRetry = getEpochTimeMillisecond() + retryAfter;
				if (!isGlobalRateLimited) {
					buckets[bucket] = nextRetry;
					onDepletedRequestSupply(bucket, retryAfter);
				}
				onExceededRateLimit(isGlobalRateLimited, retryAfter, { *this, method, path, jsonParameters, multipartParameters });
			}
			default:
			{		//error
				const ErrorCode code = static_cast<ErrorCode>(response.statusCode);
				setError(code);		//https error
				//json::Values values = json::getValues(response.text.c_str(),
				//{ "code", "message" });	//parse json to get code and message
				rapidjson::Document document;
				document.Parse(response.text.c_str());
				auto errorCode = document.FindMember("code");
				auto errorMessage = document.FindMember("message");
				if (errorCode != document.MemberEnd())
					onError(
						static_cast<ErrorCode>(errorCode->value.GetInt()),
						{ errorMessage != document.MemberEnd() ? errorMessage->value.GetString() : "" }
					);
				else
					onError(ERROR_NOTE, response.text);
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
				throw code;
#endif
			} break;
			}
			//rate limit check
			if (response.header["X-RateLimit-Remaining"] == "0" && response.statusCode != TOO_MANY_REQUESTS) {
				std::tm date = {};
				//for some reason std::get_time requires gcc 5
				std::istringstream dateStream(response.header["Date"]);
				dateStream >> std::get_time(&date, "%a, %d %b %Y %H:%M:%S GMT");
				const time_t reset = std::stoi(response.header["X-RateLimit-Reset"]);
#if defined(_WIN32) || defined(_WIN64)
				std::tm gmTM;
				std::tm*const resetGM = &gmTM;
				gmtime_s(resetGM, &reset);
#else
				std::tm* resetGM = std::gmtime(&reset);
#endif
				const time_t resetDelta = (std::mktime(resetGM) - std::mktime(&date)) * 1000;
				buckets[bucket] = resetDelta + getEpochTimeMillisecond();
				onDepletedRequestSupply(bucket, resetDelta);
			}
		}
		onResponse(response);
		return response;
	}

	Response BaseDiscordClient::request(const RequestMethod method, Route path, const std::initializer_list<Part>& multipartParameters) {
		return request(method, path, "", /*cpr::Parameters{},*/ multipartParameters);
	}/*

	Response BaseDiscordClient::request(RequestMethod method, std::string url, cpr::Parameters httpParameters) {
		return request(method, url, "", httpParameters);
	}*/

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
			*serverCache = getServers().get<std::list>();
	}

	void BaseDiscordClient::onDepletedRequestSupply(const Route::Bucket& bucket, time_t timeTilReset) {
	}

	void BaseDiscordClient::onExceededRateLimit(bool global, std::time_t timeTilRetry, Request request) {
		schedule(request, timeTilRetry);
	}

	void BaseDiscordClient::updateStatus(std::string gameName, uint64_t idleSince, Status status, bool afk) {
		std::string statusString[] = {
			"online", "dnd", "idle", "invisible", "offline"
		};

		sendL(json::createJSON({
			{ "op", json::integer(STATUS_UPDATE) },
			{ "d", json::createJSON({
				{"idle_since", idleSince != 0 ? json::UInteger(idleSince) : "null"},
				{"game", gameName != "" ? json::createJSON({
					{"name", json::string(gameName)}
				}) : "null"},
				{ "status", SleepyDiscord::json::string(statusString[status]) },
				{ "afk", SleepyDiscord::json::boolean(afk) }
			})}
		}));
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
		theGateway = "wss://gateway.discord.gg/?v=6";	//This is needed for when session is disabled
#else
		Session session;
		session.setUrl("https://discordapp.com/api/gateway");
		Response a = session.request(Get);	//todo change this back to a post
		if (!a.text.length()) {	//error check
			quit(false, true);
			return setError(GATEWAY_FAILED);
		}
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
				theGateway += "/?v=6";
				break;
			}
		}
#endif
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
		//			"$referrer":"",			//I don't know what this does
		//			"$referring_domain":""		//I don't know what this does
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
					"\"$device\":\"Sleepy_Discord\","
					"\"$referrer\":\"\","
					"\"$referring_domain\":\"\""
				"},"
				"\"compress\":false,";
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
		if (!isDisconnected) disconnectWebsocket(1000);
		if (quiting) onQuit();
	}

	void BaseDiscordClient::restart() {
		quit(true);
		connect(theGateway, this, connection);
		onRestart();
	}

	void BaseDiscordClient::reconnect(const unsigned int status) {
		if (status != 1000) {         //check for a deliberate reconnect
			heartbeatInterval = 0;    //stop heartbeating
			wasHeartbeatAcked = true; //stops the library from spamming discord
		}
		disconnectWebsocket(status);
		if (consecutiveReconnectsCount == 10) getTheGateway();
		schedule([this]() {
			connect(theGateway, this, connection);
		}, consecutiveReconnectsCount < 50 ? consecutiveReconnectsCount * 5000 : 5000 * 50);
		++consecutiveReconnectsCount;
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
		rapidjson::Document document;
		document.Parse(message.c_str(), message.length());
		//json::Values values = json::getValues(message.c_str(),
		//	{ "op", "d", "s", "t" });
		int op = document["op"].GetInt();
		const json::Value& t = document["t"];
		//const nonstd::string_view t(tValue.GetString(), tValue.GetStringLength);
		const json::Value& d = document["d"];
		switch (op) {
		case DISPATCH:
			lastSReceived = document["s"].GetInt();
			consecutiveReconnectsCount = 0; //Successfully connected
			switch (hash(json::toStdString(t).c_str())) {
			case hash("READY"                      ): {
				Ready readyData = d;
				sessionID = readyData.sessionID;
				bot = readyData.user.bot;
				userID = readyData.user;
				onReady(readyData);
				ready = true;
				} break;
			case hash("RESUMED"                    ): onResumed           (d); break;
			case hash("GUILD_CREATE"               ): {
				Server server(d);
				if (serverCache)
					serverCache->push_front(server);
				onServer(server);
				} break;
			case hash("GUILD_DELETE"               ): {
				UnavailableServer server(d);
				/*
				if (serverCache) {
					ServerCache::const_iterator foundServer = serverCache->findServer(server);
					if (foundServer != serverCache->end())
						serverCache->erase(foundServer);
				}
				*/
				if (serverCache) {
					findServerInCache(server.ID, [=](ServerCache::iterator& found) {
						serverCache->erase(found);
					});
				}
				onDeleteServer(server);
				} break;
			case hash("GUILD_UPDATE"               ): {
				Server server(d);
				/*
				if (serverCache) {
					ServerCache::iterator foundServer = serverCache->findServer(server);
					if (foundServer != serverCache->end())
						*foundServer = server;
				}
				*/
				accessServerFromCache(server.ID, [server](Server& foundServer) {
					foundServer = server;
				});
				onEditServer(server);
				} break;
			case hash("GUILD_BAN_ADD"              ): onBan  (d["guild_id"], d["user"]); break;
			case hash("GUILD_BAN_REMOVE"           ): onUnban(d["guild_id"], d["user"]); break;
			case hash("GUILD_INTEGRATIONS_UPDATE"  ):                          break; //to do add this
			case hash("GUILD_MEMBER_ADD"           ): {
				Snowflake<Server> serverID = d["guild_id"];
				ServerMember member(d);
				/*
				if (serverCache) {
					ServerCache::iterator server = serverCache->findServer(serverID);
					if (server != serverCache->end())
						server->members.push_front(member);
				}
				*/
				appendObjectToCache(serverID, &Server::members, member);
				onMember(serverID, member);
				} break;
			case hash("GUILD_MEMBER_REMOVE"        ): {
				Snowflake<Server> serverID = d["guild_id"];
				User user = d["user"];
				/*
				if (serverCache) {
					ServerCache::iterator server = serverCache->findServer(serverID);
					if (server != serverCache->end()) {
						auto member = server->findMember(user.ID);
						if (member != server->members.end())
							server->members.erase(member);
					}
				}
				*/
				eraseObjectFromCache(serverID, &Server::members, user.ID);
				onRemoveMember(serverID, user);
				} break;
			case hash("GUILD_MEMBER_UPDATE"        ): {
				Snowflake<Server> serverID = d["guild_id"];
				User user = d["user"];
				std::vector<Snowflake<Role>> roles = json::toArray<Snowflake<Role>>(d["roles"]);
				std::string nick = json::toStdString(d["nick"]);
				/*
				if (serverCache) {
					ServerCache::iterator server = serverCache->findServer(serverID);
					if (server != serverCache->end()) {
						auto foundMember = server->findMember(userID);
						if (foundMember != server->members.end()) {
							ServerMember& member = *foundMember;
							member.user = user;
							member.roles = roles;
							member.nick = nick;
						}
					}
				}
				*/
				accessObjectFromCache(serverID, &Server::members, user.ID,
					[user, roles, nick](Server& server, ServerMember& member) {
						member.user = user;
						member.roles = roles;
						member.nick = nick;
					}
				);
				onEditMember(serverID, user, roles, nick);
				} break;
			case hash("GUILD_MEMBERS_CHUNK"        ): onMemberChunk       (d["guild_id"], json::toArray<ServerMember>(d["members"])); break;
			case hash("GUILD_ROLE_CREATE"          ): {
				Snowflake<Server> serverID = d["guild_id"];
				Role role = d["role"];
				/*
				if (serverCache) {
					ServerCache::iterator server = serverCache->findServer(serverID);
					if (server != serverCache->end())
						server->roles.push_front(role);
				}
				*/
				appendObjectToCache(serverID, &Server::roles, role);
				onRole(serverID, role);
				} break;
			case hash("GUILD_ROLE_UPDATE"):
			{
				Snowflake<Server> serverID = d["guild_id"];
				Role role = d["role"];
				/*
				if (serverCache) {
					ServerCache::iterator server = serverCache->findServer(serverID);
					if (server != serverCache->end()) {
						auto foundRole = server->findRole(role);
						if (foundRole != server->roles.end())
							*foundRole = role;
					}
				}
				*/
				accessObjectFromCache(serverID, &Server::roles, role.ID,
					[role](Server& server, Role& foundRole) {
						foundRole = role;
					}
				);
				onEditRole(serverID, role);
			} break;
			case hash("GUILD_ROLE_DELETE"          ): {
				Snowflake<Server> serverID = d["guild_id"];
				Snowflake<Role> roleID = d["role_id"];
				/*
				if (serverCache) {
					ServerCache::iterator server = serverCache->findServer(serverID);
					if (server != serverCache->end()) {
						auto foundRole = server->findRole(roleID);
						if (foundRole != server->roles.end())
							server->roles.erase(foundRole);
					}
				}
				*/
				eraseObjectFromCache(serverID, &Server::roles, roleID);
				onDeleteRole(serverID, roleID);
				} break;
			case hash("GUILD_EMOJIS_UPDATE"        ): onEditEmojis        (d["guild_id"], json::toArray<Emoji>(d["emojis"])); break;
			case hash("CHANNEL_CREATE"             ): {
				Channel channel = d;
				/*
				if (serverCache) {
					ServerCache::iterator server = serverCache->findServer(channel.serverID);
					if (server != serverCache->end())
						server->channels.push_front(channel);
				}
				*/
				appendObjectToCache(channel.serverID, &Server::channels, channel);
				onChannel(d);
				} break;
			case hash("CHANNEL_UPDATE"             ): {
				Channel channel = d;
				/*
				if (serverCache) {
					ServerCache::iterator server = serverCache->findServer(channel.serverID);
					if (server != serverCache->end()) {
						auto foundChannel = server->findChannel(channel);
						if (foundChannel != server->channels.end())
							*foundChannel = channel;
					}
				}
				*/
				accessObjectFromCache(channel.serverID, &Server::channels, channel.ID,
					[channel](Server& server, Channel& foundChannel) {
						foundChannel = channel;
					}
				);
				onEditChannel(d); 
				} break;
			case hash("CHANNEL_DELETE"             ): {
				Channel channel = d;
				/*
				if (serverCache) {
					ServerCache::iterator server = serverCache->findServer(channel.serverID);
					if (server != serverCache->end()) {
						auto foundChannel = server->findChannel(channel);
						if (foundChannel != server->channels.end())
							server->channels.erase(foundChannel);
					}
				}
				*/
				eraseObjectFromCache(channel.serverID, &Server::channels, channel.ID);
				onDeleteChannel(d);
				} break;
			case hash("CHANNEL_PINS_UPDATE"        ): onPinMessage        (d["channel_id"], json::toStdString(d["last_pin_timestamp"])); break;
			case hash("PRESENCE_UPDATE"            ): onPresenceUpdate    (d); break;
			case hash("PRESENCES_REPLACE"          ):                          break;
			case hash("USER_UPDATE"                ): onEditUser          (d); break;
			case hash("USER_NOTE_UPDATE"           ): onEditUserNote      (d); break;
			case hash("USER_SETTINGS_UPDATE"       ): onEditUserSettings  (d); break;
			case hash("VOICE_STATE_UPDATE"         ): {
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
			case hash("TYPING_START"               ): onTyping            (d["channel_id"], d["user_id"], d["timestamp"].GetInt64() * 1000); break;
			case hash("MESSAGE_CREATE"             ): onMessage           (d); break;
			case hash("MESSAGE_UPDATE"             ): onEditMessage       (d); break;
			case hash("MESSAGE_DELETE"             ): onDeleteMessages    (d["channel_id"], { d["id"] }); break;
			case hash("MESSAGE_DELETE_BULK"        ): onDeleteMessages    (d["channel_id"], json::toArray<Snowflake<Message>>(d["ids"])); break;
			case hash("VOICE_SERVER_UPDATE"        ): {
				VoiceServerUpdate voiceServer(d);
#ifdef SLEEPY_VOICE_ENABLED
				if (!waitingVoiceContexts.empty()) {
					auto iterator = find_if(waitingVoiceContexts.begin(), waitingVoiceContexts.end(),
						[&voiceServer](const VoiceContext* w) {
						return voiceServer.serverID == w->serverID && w->endpoint == "";
					});
					if (iterator != waitingVoiceContexts.end()) {
						VoiceContext& context = **iterator;
						context.token    = voiceServer.token;
						context.endpoint = voiceServer.endpoint;
						connectToVoiceIfReady(context);
					}
				}
#endif
				onEditVoiceServer(voiceServer);
				} break;
			case hash("GUILD_SYNC"                 ): onServerSync        (d); break;
			case hash("RELATIONSHIP_ADD"           ): onRelationship      (d); break;
			case hash("RELATIONSHIP_REMOVE"        ): onDeleteRelationship(d); break;
			case hash("MESSAGE_REACTION_ADD"       ): onReaction          (d["user_id"], d["channel_id"], d["message_id"], d["emoji"]); break;
			case hash("MESSAGE_REACTION_REMOVE"    ): onDeleteReaction    (d["user_id"], d["channel_id"], d["message_id"], d["emoji"]); break;
			case hash("MESSAGE_REACTION_REMOVE_ALL"): onDeleteAllReaction (d["user_id"], d["channel_id"]); break;
			default: 
				setError(EVENT_UNKNOWN);
				onError(ERROR_NOTE, json::toStdString(t));
				break;
			}
			onDispatch(d);
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
			if (d[0][0] == 't') {
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
			return quit(false, true);
			break;
		}
		reconnect(1001);
	}

	void BaseDiscordClient::heartbeat() {
		if (heartbeatInterval <= 0 || isQuiting()) return; //sanity test
		
		//if time and timer are out of sync, trust time
		time_t currentTime = getEpochTimeMillisecond();
		time_t nextHeartbest;
		if (currentTime <= (nextHeartbest = lastHeartbeat + heartbeatInterval)) {
			heart = schedule(&BaseDiscordClient::heartbeat, nextHeartbest - currentTime);
			return;
		}

		if (!wasHeartbeatAcked) {
			reconnect(1001);
		} else {
			sendHeartbeat();
		}

		lastHeartbeat = currentTime;

		heart = schedule(&BaseDiscordClient::heartbeat, heartbeatInterval);
	}

	void BaseDiscordClient::sendHeartbeat() {
		std::string str = std::to_string(lastSReceived);
		std::string heartbeat;
		//The number 18 comes from 1 plus the length of {\"op\":1,\"d\":}
		heartbeat.reserve(18 + str.length());
		heartbeat += 
			"{"
				"\"op\":1,"
				"\"d\":"; heartbeat += str; heartbeat +=
			"}";
		sendL(heartbeat);
		wasHeartbeatAcked = false;
		onHeartbeat();
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

		std::string endpoint;
		//Discord doens't gives the endpoint with wss:// or ?v=3, so it's done here
		//length of wss:///?v=3 is 11, plus one equals 12
		endpoint.reserve(12 + givenEndpoint.length());
		endpoint += "wss://";
		endpoint += givenEndpoint;
		endpoint += "/?v=3";

		//Add a new connection to the list of connections
		voiceConnections.emplace_front( this, context );
		VoiceConnection& voiceConnection = voiceConnections.back();

		connect(endpoint, &voiceConnection, voiceConnection.connection);

		//remove from wait list
		waitingVoiceContexts.remove(&context);
	}

	void BaseDiscordClient::removeVoiceConnectionAndContext(VoiceConnection & connection) {
		const VoiceContext& context = connection.getContext();
		voiceConnections.remove_if(
			[&connection](VoiceConnection& right) {
				return connection == right;
			}
		);
		voiceContexts.remove(context);
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
		constexpr char channelIDIdentifier[] = "channel.id";
		constexpr char serverIDIdentifier[] = "guild.id";

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

			if (identifier == channelIDIdentifier)
				channelID = replacement;
			else if (identifier == serverIDIdentifier)
				serverID = replacement;

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
		target.reserve(methodString.length() + channelID.string().length()
			+ serverID.string().length() + path.length());
		target += methodString;
		target += channelID;
		target += serverID;
		target += path;
		return target;
	}
}
