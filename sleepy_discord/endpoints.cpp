#include <functional>
#include "client.h"
#include "discord_object_interface.h"
#include "vector"

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning( disable: 4458 ) //warns about variables that hide class members
#endif

//important note, all requests on sync mode throw on an http error

namespace SleepyDiscord {
	void BaseDiscordClient::testFunction(std::string teststring) {

	}
	//
	//channel functions
	//

	//ObjectResponse<Message> BaseDiscordClient::sendMessage(Snowflake<Channel> channelID, CreateMessageParams& params) {
	//	return ObjectResponse<Message>{ request(Post, path("channels/{channel.id}/messages", { channelID }), json::stringifyObj(params)) };
	//}

	ObjectResponse<Message> BaseDiscordClient::sendMessage(Snowflake<Channel> channelID, std::string message, Embed embed, bool tts, RequestSettings<ObjectResponse<Message>> settings) {
		rapidjson::Document doc;
		doc.SetObject();
		rapidjson::Value content;
		auto& allocator = doc.GetAllocator();
		content.SetString(message.c_str(), message.length());
		doc.AddMember("content", content, allocator);
		if (tts == true) doc.AddMember("tts", true, allocator);
		if (!embed.empty()) doc.AddMember("embed", json::toJSON(embed, allocator), allocator);
		return ObjectResponse<Message>{ request(Post, path("channels/{channel.id}/messages", { channelID }), settings, json::stringify(doc)) };
	}

	ObjectResponse<Message> BaseDiscordClient::sendMessage(SendMessageParams params, RequestSettings<ObjectResponse<Message>> settings) {
		return ObjectResponse<Message>{ request(Post, path("channels/{channel.id}/messages", { params.channelID }), settings, json::stringify(json::toJSON(params))) };
	}

	ObjectResponse<Message> BaseDiscordClient::uploadFile(Snowflake<Channel> channelID, std::string fileLocation, std::string message, RequestSettings<ObjectResponse<Message>> settings) {
		return ObjectResponse<Message>{
			request(Post, path("channels/{channel.id}/messages", { channelID }), settings, "", {
				{ "content", message },
				{ "file", filePathPart{fileLocation} }
			})
		};
	}

	ObjectResponse<Message> BaseDiscordClient::editMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string newMessage, RequestSettings<ObjectResponse<Message>> settings) {
		rapidjson::Document doc;
		doc.SetObject();
		rapidjson::Value content;
		content.SetString(newMessage.c_str(), newMessage.length());
		doc.AddMember("content", content, doc.GetAllocator());
		return ObjectResponse<Message>{ request(Patch, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }), settings, json::stringify(doc)) };
	}

	BoolResponse BaseDiscordClient::deleteMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::bulkDeleteMessages(Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messageIDs, RequestSettings<BoolResponse> settings) {
		std::string JSON = "{\"messages\":[";
		for (Snowflake<Message> messageID : messageIDs) {
			JSON += messageID;
			JSON += ',';
		}
		if (messageIDs.size())
			JSON.pop_back();
		JSON += "]}";
		return { request(Post, path("channels/{channel.id}/messages/bulk-delete", { channelID }), settings, JSON), EmptyRespFn() };
	}

	ObjectResponse<Channel> BaseDiscordClient::editChannel(Snowflake<Channel> channelID, std::string name, std::string topic, RequestSettings<ObjectResponse<Channel>> settings) {
		rapidjson::Document doc;
		doc.SetObject();
		auto& allocator = doc.GetAllocator();
		rapidjson::Value nameValue;
		if (name != "") {
			nameValue.SetString(name.c_str(), name.length());
			doc.AddMember("name", nameValue, allocator);
		}
		rapidjson::Value topicValue;
		if (topic != "") {
			topicValue.SetString(topic.c_str(), topic.length());
			doc.AddMember("topic", topicValue, allocator);
		}
		return ObjectResponse<Channel>{ request(Patch, path("channels/{channel.id}", { channelID }), settings, json::stringify(doc)) };
	}

	ObjectResponse<Channel> BaseDiscordClient::editChannelName(Snowflake<Channel> channelID, std::string name, RequestSettings<ObjectResponse<Channel>> settings) {
		return editChannel(channelID, name, "", settings);
	}

	ObjectResponse<Channel> BaseDiscordClient::editChannelTopic(Snowflake<Channel> channelID, std::string topic, RequestSettings<ObjectResponse<Channel>> settings) {
		return editChannel(channelID, "", topic, settings);
	}

	ObjectResponse<Channel> BaseDiscordClient::deleteChannel(Snowflake<Channel> channelID, RequestSettings<ObjectResponse<Channel>> settings) {
		return ObjectResponse<Channel>{ request(Delete, path("channels/{channel.id}", { channelID }), settings) };
	}

	ObjectResponse<Channel> BaseDiscordClient::getChannel(Snowflake<Channel> channelID, RequestSettings<ObjectResponse<Channel>> settings) {
		return ObjectResponse<Channel>{ request(Get, path("channels/{channel.id}", { channelID }), settings) };
	}

	ArrayResponse<Message> BaseDiscordClient::getMessages(Snowflake<Channel> channelID, GetMessagesKey when, Snowflake<Message> messageID, uint8_t _limit) {
		const uint8_t trueLimit = 100 < _limit ? 100 : _limit;
		std::string key;
		switch (when) {
		case around: key = "?around=" + messageID; break;
		case before: key = "?before=" + messageID; break;
		case after:  key = "?after="  + messageID; break;
		case limit:  key = "?"                    ; break;
		default:     key = ""                     ; break;
		}
		if (trueLimit != 0 && when != limit) key += '&';
		return ArrayResponse<Message>{
			request(Get,
				path("channels/{channel.id}/messages{key}{limit}", { channelID, key,
				(trueLimit != 0 ? "limit=" + std::to_string(trueLimit) : "") })
			)
		};
	}

	ObjectResponse<Message> BaseDiscordClient::getMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<ObjectResponse<Message>> settings) {
		return ObjectResponse<Message>{ request(Get, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }), settings) };
	}

	BoolResponse BaseDiscordClient::addReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, RequestSettings<BoolResponse> settings) {
		return { request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/@me", { channelID, messageID, emoji }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::removeReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, Snowflake<User> userID) {
		return { request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/{user.id}", { channelID, messageID, emoji, userID })), EmptyRespFn() };
	}

	ArrayResponse<Reaction> BaseDiscordClient::getReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, RequestSettings<ArrayResponse<Reaction>> settings) {
		return ArrayResponse<Reaction>{ request(Get, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}", { channelID, messageID, emoji }), settings) };
	}

	StandardResponse BaseDiscordClient::removeAllReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<StandardResponse> settings) {
		return StandardResponse{ request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions", { channelID, messageID }), settings) };
	}

	BoolResponse BaseDiscordClient::editChannelPermissions(Snowflake<Channel> channelID, Snowflake<Overwrite> overwriteID, int allow, int deny, std::string type) {
		return { request(
			Put,
			path("channels/{channel.id}/permissions/{overwrite.id}", { channelID, overwriteID }),
			json::createJSON({
				{ "allow", std::to_string(allow) },
				{ "deny", std::to_string(deny) },
				{ "type", json::string(type) }
		})), EmptyRespFn() };
	}

	ArrayResponse<Invite> BaseDiscordClient::getChannelInvites(Snowflake<Channel> channelID, RequestSettings<ArrayResponse<Invite>> settings) {
		return ArrayResponse<Invite>{ request(Get, path("channels/{channel.id}/invites", { channelID }), settings) };
	}

	ObjectResponse<Invite> BaseDiscordClient::createChannelInvite(Snowflake<Channel> channelID, const uint64_t maxAge, const uint64_t maxUses, const bool temporary, const bool unique) {
		return ObjectResponse<Invite>{
			request(Post, path("channels/{channel.id}/invites", { channelID }),
				json::createJSON({
					{"max_age", json::optionalUInteger(maxAge) },
					{"max_uses", json::optionalUInteger(maxUses) },
					{ "temporary", temporary ? "true" : ""},
					{ "unique", unique ? "true" : ""}
				}
			))
		};
	}

	BoolResponse BaseDiscordClient::removeChannelPermission(Snowflake<Channel> channelID, std::string ID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("channels/{channel.id}/permissions/{overwrite.id}", { channelID, ID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::sendTyping(Snowflake<Channel> channelID, RequestSettings<BoolResponse> settings) {
		return { request(Post, path("channels/{channel.id}/typing", { channelID }), settings), EmptyRespFn() };
	}

	ArrayResponse<Message> BaseDiscordClient::getPinnedMessages(Snowflake<Channel> channelID, RequestSettings<ArrayResponse<Message>> settings) {
		return ArrayResponse<Message>{ request(Get, path("channels/{channel.id}/pins", { channelID }), settings) };
	}

	BoolResponse BaseDiscordClient::pinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<BoolResponse> settings) {
		return { request(Put, path("channels/{channel.id}/pins/{message.id}", { channelID, messageID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::unpinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("channels/{channel.id}/pins/{message.id}", { channelID, messageID }), settings), EmptyRespFn() };
	}

	StandardResponse BaseDiscordClient::addRecipient(Snowflake<Channel> channelID, Snowflake<User> userID, RequestSettings<StandardResponse> settings) {
		return StandardResponse{ request(Put, path("channels/{channel.id}/recipients/{user.id}", { channelID, userID }), settings) };
	}

	StandardResponse BaseDiscordClient::removeRecipient(Snowflake<Channel> channelID, Snowflake<User> userID, RequestSettings<StandardResponse> settings) {
		return StandardResponse{ request(Delete, path("channels/{channel.id}/recipients/{user.id}", { channelID, userID }), settings) };
	}

	//
	//server functions
	//
	ObjectResponse<Channel> BaseDiscordClient::createTextChannel(Snowflake<Server> serverID, std::string name, RequestSettings<ObjectResponse<Channel>> settings) {
		return ObjectResponse<Channel>{
			request(Post, path("guilds/{guild.id}/channels", { serverID }), settings, "{\"name\": " + json::string(name) + ", \"type\": \"text\"}")
		};
	}

	ArrayResponse<Channel> BaseDiscordClient::editChannelPositions(Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions, RequestSettings<ArrayResponse<Channel>> settings) {
		return ArrayResponse<Channel>{ request(Patch, path("guilds/{guild.id}/channels", { serverID }), getEditPositionString(positions)) };
	}

	ObjectResponse<ServerMember> SleepyDiscord::BaseDiscordClient::getMember(Snowflake<Server> serverID, Snowflake<User> userID, RequestSettings<ObjectResponse<ServerMember>> settings) {
		return ObjectResponse<ServerMember>{ request(Get, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), settings) };
	}

	ArrayResponse<ServerMember> BaseDiscordClient::listMembers(Snowflake<Server> serverID, uint16_t limit, std::string after, RequestSettings<ArrayResponse<ServerMember>> settings) {
		//Todo better Query String Params support
		std::string limitParm = limit != 0 ? "?limit=" + std::to_string(limit) : "";
		std::string afterParm = after != "" ? "after=" + after : "";
		if (afterParm != "" && limitParm != "") limitParm += '&';
		else if (afterParm != "" && limitParm == "") limitParm += '?';
		return ArrayResponse<ServerMember>{ request(Get, path("guilds/{guild.id}/members{limit}{after}", { serverID, limitParm, afterParm }), settings) };
	}

	ObjectResponse<ServerMember> BaseDiscordClient::addMember(Snowflake<Server> serverID, Snowflake<User> userID, std::string accesToken, std::string nick, std::vector<Role> roles, bool mute, bool deaf) {
		std::string rolesString = "";
		if (roles.empty()){
			rolesString = "";
		} else {
			int i = -1;
			std::vector<std::string> values(roles.size());
			for (Role role : roles) {
				values[++i] = json::createJSON({
					{ "id"         , json::string  (role.ID         ) },
					{ "name"       , json::string  (role.name       ) },
					{ "color"      , json::UInteger(role.color      ) },
					{ "hoist"      , json::boolean (role.hoist      ) },
					{ "position"   , json::integer (role.position   ) },
					{ "managed"    , json::boolean (role.managed    ) },
					{ "mentionable", json::boolean (role.mentionable) }
				});
			}
			rolesString = json::createJSONArray(values);
		}

		return ObjectResponse<ServerMember>{
			request(Put, path("guilds/{guild.id}/members/{user.id}"), json::createJSON({
				{ "access_token", json::string (accesToken) },
				{ "nick"        , json::string (nick)       },
				{ "roles"       , rolesString               },
				{ "mute"        , json::boolean(mute)       },
				{ "deaf"        , json::boolean(deaf)       }
			})) 
		};
	}

	BoolResponse BaseDiscordClient::editMember(Snowflake<Server> serverID, Snowflake<User> userID, std::string nickname, std::vector<Snowflake<Role>> roles, int8_t mute, int8_t deaf, Snowflake<Channel> channelID) {
		const std::string muteString = mute == -1 ? json::boolean(mute) : "";
		const std::string deafString = deaf == -1 ? json::boolean(deaf) : "";

		return { request(Patch, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), json::createJSON({
			{ "nick"      , json::string(nickname)       },
			{ "roles"     , json::createJSONArray(roles) },
			{ "mute"      , muteString                   },
			{ "deaf"      , deafString                   },
			{ "channel_id", channelID                    },
		})), EmptyRespFn() };
	}

	ArrayResponse<Role> BaseDiscordClient::editRolePosition(Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions, RequestSettings<ArrayResponse<Role>> settings) {
		return ArrayResponse<Role>{ request(Patch, path("guilds/{guild.id}/roles", { serverID }), settings, getEditPositionString(positions)) };
	}


	StringResponse BaseDiscordClient::editRole(Snowflake<Server> serverID, Snowflake<Role> roleID, std::string name, Permission permissions, uint32_t color, int8_t hoist, int8_t mentionable) {
		const std::string colorString       = color       >> 24 == 0 ? std::to_string(color      ) : "";	//if over 24 bits, do not change color
		const std::string hoistString       = hoist       >> 1  == 0 ? json::boolean (hoist      ) : "";	//if larger then 1 bit, do change hoist
		const std::string mentionableString = mentionable >> 1  == 0 ? json::boolean (mentionable) : "";

		return StringResponse{
			request(Patch, path("guilds/{guild.id}/roles/{role.id}", { serverID, roleID }), json::createJSON({
				{ "name"       , json::string(name)         },
				{ "permissions", json::integer(permissions) },
				{ "color"      , colorString                },
				{ "hoist"      , hoistString                },
				{ "mentionable", mentionableString          }
			}))
		};
	}

	BoolResponse SleepyDiscord::BaseDiscordClient::deleteRole(Snowflake<Server> serverID, Snowflake<Role> roleID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/roles/{role.id}", { serverID, roleID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::muteServerMember(Snowflake<Server> serverID, Snowflake<User> userID, bool mute, RequestSettings<BoolResponse> settings) {
		return { request(Patch, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), settings, mute ? "{\"mute\":true}" : "{\"mute\":false}"), EmptyRespFn() };
	}

	//needs ablily to turn channel into json
	/*ObjectResponse<Server> BaseDiscordClient::createServer(std::string name, std::string region, std::string icon, int verificationLevel, int defaultMessageNotifications, int explicitContentLevel, std::vector<Role> roles, std::vector<Channel> channels) {
		request(Post, "guilds", json::createJSON({
			{ "name"                         , json::string (name) },
			{ "region"                       , json::string(region) },
			{ "icon"                         , json::string(icon) },
			{ "verification_level"           , json::integer(verificationLevel) },
			{ "default_message_notifications", json::integer(defaultMessageNotifications) },
			{ "explicit_content_filter"      , json::integer(explicitContentLevel) },
			{ "roles"                        , json::createJSONArray(roles) },
			{ "channels"                     , json::createJSONArray(channels) }
		}));
	}*/

	ObjectResponse<Server> BaseDiscordClient::getServer(Snowflake<Server> serverID, RequestSettings<ObjectResponse<Server>> settings) {
		return ObjectResponse<Server>{ request(Get, path("guilds/{guild.id}", { serverID }), settings) };
	}

	ObjectResponse<Server> BaseDiscordClient::deleteServer(Snowflake<Server> serverID, RequestSettings<ObjectResponse<Server>> settings) {
		return ObjectResponse<Server>{ request(Delete, path("guilds/{guild.id}", { serverID }), settings) };
	}

	ArrayResponse<Channel> SleepyDiscord::BaseDiscordClient::getServerChannels(Snowflake<Server> serverID, RequestSettings<ArrayResponse<Channel>> settings) {
		return ArrayResponse<Channel>{ request(Get, path("guilds/{guild.id}/channels", { serverID }), settings) };
	}

	BoolResponse BaseDiscordClient::editNickname(Snowflake<Server> serverID, std::string newNickname, RequestSettings<BoolResponse> settings) {
		return { request(Patch, path("guilds/{guild.id}/members/@me/nick", { serverID }), settings, "{\"nick\":" + json::string(newNickname) + "}"), StandardRespFn() };
	}

	BoolResponse BaseDiscordClient::addRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID, RequestSettings<BoolResponse> settings) {
		return { request(Put, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", { serverID, userID, roleID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::removeRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", { serverID, userID, roleID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::kickMember(Snowflake<Server> serverID, Snowflake<User> userID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), settings), EmptyRespFn() };
	}

	ArrayResponse<User> BaseDiscordClient::getBans(Snowflake<Server> serverID, RequestSettings<ArrayResponse<User>> settings) {
		return ArrayResponse<User>{ request(Get, path("guilds/{guild.id}/bans", { serverID }), settings) };
	}

	BoolResponse BaseDiscordClient::banMember(Snowflake<Server> serverID, Snowflake<User> userID, RequestSettings<BoolResponse> settings) {
		return { request(Put, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::unbanMember(Snowflake<Server> serverID, Snowflake<User> userID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID }), settings), EmptyRespFn() };
	}

	ArrayResponse<Role> BaseDiscordClient::getRoles(Snowflake<Server> serverID, RequestSettings<ArrayResponse<Role>> settings) {
		return ArrayResponse<Role>{ request(Get, path("guilds/{guild.id}/roles", { serverID }), settings) };
	}

	ObjectResponse<Role> BaseDiscordClient::createRole(Snowflake<Server> serverID, std::string name, Permission permissions, unsigned int color, bool hoist, bool mentiionable) {
		return ObjectResponse<Role>{
			request(Post, path("guilds/{guild.id}/roles", { serverID }), json::createJSON({
				{ "name"       , json::string (name        ) },
				{ "permissions", json::integer(permissions ) },
				{ "color"      , json::integer(color       ) },
				{ "hoist"      , json::boolean(hoist       ) },
				{ "mentionable", json::boolean(mentiionable) }
			}))
		};
	}

	StandardResponse BaseDiscordClient::pruneMembers(Snowflake<Server> serverID, const unsigned int numOfDays, RequestSettings<StandardResponse> settings) {
		if (numOfDays == 0) return StandardResponse{ BAD_REQUEST };
		return StandardResponse{ request(Post, path("guilds/{guild.id}/prune", { serverID }), settings, "{\"days\":" + std::to_string(numOfDays) + '}') };
	}

	ArrayResponse<VoiceRegion> BaseDiscordClient::getVoiceRegions(RequestSettings<ArrayResponse<VoiceRegion>> settings) {
		return ArrayResponse<VoiceRegion>{ request(Get, path("guilds/{guild.id}/regions"), settings) };
	}

	ArrayResponse<Invite> BaseDiscordClient::getServerInvites(Snowflake<Server> serverID, RequestSettings<ArrayResponse<Invite>> settings) {
		return ArrayResponse<Invite>{ request(Get, path("guilds/{guild.id}/invites", { serverID }), settings) };
	}

	StringResponse BaseDiscordClient::getIntegrations(Snowflake<Server> serverID, RequestSettings<StringResponse> settings) {
		return StringResponse{ request(Get, path("guilds/{guild.id}/integrations", { serverID }), settings) };
	}

	BoolResponse BaseDiscordClient::createIntegration(Snowflake<Server> serverID, std::string type, std::string integrationID, RequestSettings<BoolResponse> settings) {
		return { request(Post, path("guilds/{guild.id}/integrations", { serverID }), settings, json::createJSON({
			{ "type", json::string(type) },
			{ "id", json::string(integrationID) }
		})), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::editIntergration(Snowflake<Server> serverID, std::string integrationID, int expireBegavior, int expireGracePeriod, bool enbleEmoticons) {
		return { request(Patch, path("guilds/{guild.id}/integrations/{integration.id}", { serverID, integrationID }), json::createJSON({
			{ "expire_behavior", json::integer(expireBegavior) },
			{ "expire_grace_period", json::integer(expireGracePeriod) },
			{ "enable_emoticons", json::boolean(enbleEmoticons) }
		})), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::deleteIntegration(Snowflake<Server> serverID, std::string integrationID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/integrations/{integration.id}", { serverID, integrationID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::syncIntegration(Snowflake<Server> serverID, std::string integrationID, RequestSettings<BoolResponse> settings) {
		return { request(Post, path("guilds/{guild.id}/integrations/{integration.id}/sync", { serverID, integrationID }), settings), EmptyRespFn() };
	}

	ObjectResponse<ServerEmbed> BaseDiscordClient::getServerEmbed(Snowflake<Server> serverID, RequestSettings<ObjectResponse<ServerEmbed>> settings) {
		return ObjectResponse<ServerEmbed>{ request(Get, path("guilds/{guild.id}/embed", { serverID }), settings) };
	}

	//
	//Invite functions
	//
	ObjectResponse<Invite> BaseDiscordClient::inviteEndpoint(RequestMethod method, std::string inviteCode, RequestSettings<ObjectResponse<Invite>> settings) {
		return ObjectResponse<Invite>{ request(method, path("invites/{invite.code}", { inviteCode }), settings) };
	}

	ObjectResponse<Invite> BaseDiscordClient::getInvite(std::string inviteCode, RequestSettings<ObjectResponse<Invite>> settings) {
		return inviteEndpoint(Get, inviteCode, settings);
	}

	ObjectResponse<Invite> BaseDiscordClient::deleteInvite(std::string inviteCode, RequestSettings<ObjectResponse<Invite>> settings) {
		return inviteEndpoint(Delete, inviteCode, settings);
	}

	ObjectResponse<Invite> BaseDiscordClient::acceptInvite(std::string inviteCode, RequestSettings<ObjectResponse<Invite>> settings) {
		return inviteEndpoint(Post, inviteCode, settings);
	}
	//
	//User functions
	//
	ObjectResponse<User> BaseDiscordClient::getCurrentUser(RequestSettings<ObjectResponse<User>> settings) {
		return ObjectResponse<User>{ request(Get, "users/@me", settings) };
	}

	ObjectResponse<User> BaseDiscordClient::getUser(Snowflake<User> userID, RequestSettings<ObjectResponse<User>> settings) {
		return ObjectResponse<User>{ request(Get, path("users/{user.id}", { userID }), settings) };
	}

	ArrayResponse<Server> BaseDiscordClient::getServers(RequestSettings<ArrayResponse<Server>> settings) {
		return ArrayResponse<Server>{ request(Get, "users/@me/guilds", settings) };
	}

	BoolResponse BaseDiscordClient::leaveServer(Snowflake<Server> serverID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("users/@me/guilds/{guild.id}", { serverID }), settings), EmptyRespFn() };
	}

	ArrayResponse<Channel> BaseDiscordClient::getDirectMessageChannels(RequestSettings<ArrayResponse<Channel>> settings) {
		return ArrayResponse<Channel>{ request(Get, "users/@me/channels", settings) };
	}

	ObjectResponse<Channel> BaseDiscordClient::createDirectMessageChannel(std::string recipientID, RequestSettings<ObjectResponse<Channel>> settings) {
		return ObjectResponse<Channel>{ request(Post, "users/@me/channels", settings, json::createJSON({ { "recipient_id", recipientID } })) };
	}

	ArrayResponse<Connection> BaseDiscordClient::getUserConnections(RequestSettings<ArrayResponse<Connection>> settings) {
		return ArrayResponse<Connection>{ request(Get, "users/@me/connections", settings) };
	}

	//
	//Webhook functions
	//
	ObjectResponse<Webhook> BaseDiscordClient::createWebhook(Snowflake<Channel> channelID, std::string name, std::string avatar, RequestSettings<ObjectResponse<Webhook>> settings) {
		return ObjectResponse<Webhook>{ request(Post, path("channels/{channel.id}/webhooks", { channelID }), settings, json::createJSON({
			{"name", json::string(name)},
			{"avatar", json::string(avatar)}
		})) };
	}

	ArrayResponse<Webhook> BaseDiscordClient::getChannelWebhooks(Snowflake<Channel> channelID, RequestSettings<ArrayResponse<Webhook>> settings) {
		return ArrayResponse<Webhook>{ request(Get, path("channels/{channel.id}/webhooks", { channelID }), settings) };
	}

	ArrayResponse<Webhook> BaseDiscordClient::getServerWebhooks(Snowflake<Server> serverID, RequestSettings<ArrayResponse<Webhook>> settings) {
		return ArrayResponse<Webhook>{ request(Get, path("guilds/{guild.id}/webhooks", { serverID }), settings) };
	}

	inline const char* optionalWebhookToken(std::string webhookToken) {
		return webhookToken != "" ? "webhooks/{webhook.id}/{webhook.token}" : "webhooks/{webhook.id}";
	}

	ObjectResponse<Webhook> BaseDiscordClient::getWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, RequestSettings<ObjectResponse<Webhook>> settings) {
		return ObjectResponse<Webhook>{ request(Get, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }), settings) };
	}

	ObjectResponse<Webhook> BaseDiscordClient::editWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::string name, std::string avatar) {
		return ObjectResponse<Webhook>{ request(Patch, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }), json::createJSON({
			{ "name", json::string(name) },
			{ "avatar", json::string(avatar) }
		})) };
	}

	BoolResponse BaseDiscordClient::deleteWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }), settings), EmptyRespFn() };
	}

	//excute webhook

	ObjectResponse<Webhook> BaseDiscordClient::requestExecuteWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::pair<std::string, std::string> pair, bool wait, std::string username, std::string avatar_url, bool tts) {
		return ObjectResponse<Webhook>{
			request(Post, path("webhooks/{webhook.id}/{webhook.token}{wait}", { webhookID, webhookToken, (wait ? "?around=true" : "") }), json::createJSON({
				pair,
				{ "username"  , json::string(username  ) },
				{ "avatar_url", json::string(avatar_url) },
				{ "tts"       , (tts ? "true" : "")      }
			})) };
	}

	ObjectResponse<Webhook> BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::string content, bool wait, std::string username, std::string avatar_url, bool tts) {
		return ObjectResponse<Webhook>{ requestExecuteWebhook(webhookID, webhookToken, { "content", json::string(content) }, wait, username, avatar_url, tts) };
	}

	//Webhook SleepyDiscord::BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::vector<Embed> embeds, bool wait, std::string username, std::string avatar_url bool tts) {
	//	
	//	return requestExecuteWebhook(webhookID, webhookToken, { "embeds", crazy stuff happens here }, wait, username, avatar_url, tts);
	//}
	
	ObjectResponse<Webhook> BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, filePathPart file, bool /*wait*/, std::string username, std::string avatar_url, bool tts) {
		return ObjectResponse<Webhook>{ request(Post, path("webhooks/{webhook.id}/{webhook.token}", { webhookID, webhookToken }), "", {
			{ "file"      , filePathPart(file)  },
			{ "username"  , username            },
			{ "avatar_url", avatar_url          },
			{ "tts"       , (tts ? "true" : "") }
		}) };
	}
}