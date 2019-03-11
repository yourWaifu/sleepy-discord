#include "client.h"
#include "discord_object_interface.h"
#include "vector"

namespace SleepyDiscord {
	void BaseDiscordClient::testFunction(std::string teststring) {
		//request(Post, path("guilds/{guild.id}/roles", std::string("202917641101246465")), json::createJSON({
		//		{"color", json::UInteger(0x1000000)}
		//}));

		//ObjectResponse<Message> m = sendMessage("error_test", "testing");
		//if (m.error()) {
		//	onError(ERROR_ZERO, "test");
		//}
		//if (std::vector<Channel>(getServerChannels("error_test")).empty()) {
		//	onError(ERROR_NOTE, "test");
		//}

		//sendMessage(createDirectMessageChannel("99259409045143552")->ID, "Hey, How's it going?");
		
		//uploadFile("202917641101246465", "C:/Users/steve/Documents/nsprojects/sleepy_discord/test/test_windows/hello.png", "Hello World");
		
		auto user = getCurrentUser();
		User currentUser = user;

		Embed embed;
		embed.title = "HELLO!!!";
		sendMessage(createDirectMessageChannel("99259409045143552").cast().ID, "", embed);
	}
	//
	//channel functions
	//
	ObjectResponse<Message> BaseDiscordClient::sendMessage(Snowflake<Channel> channelID, std::string message, Embed embed, bool tts) {
		rapidjson::Document doc;
		doc.SetObject();
		rapidjson::Value content;
		auto& allocator = doc.GetAllocator();
		content.SetString(message.c_str(), message.length());
		doc.AddMember("content", content, allocator);
		if (tts == true) doc.AddMember("tts", true, allocator);
		if (!embed.empty()) doc.AddMember("embed", json::toJSON(embed, allocator), allocator);
		return request(Post, path("channels/{channel.id}/messages", { channelID }), json::stringify(doc));
	}

	ObjectResponse<Message> BaseDiscordClient::uploadFile(Snowflake<Channel> channelID, std::string fileLocation, std::string message) {
		return request(Post, path("channels/{channel.id}/messages", { channelID }),
			{ { "content", message },
			{ "file", filePathPart{fileLocation} }
		}
		);
	}

	ObjectResponse<Message> BaseDiscordClient::editMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string newMessage) {
		rapidjson::Document doc;
		doc.SetObject();
		rapidjson::Value content;
		content.SetString(newMessage.c_str(), newMessage.length());
		doc.AddMember("content", content, doc.GetAllocator());
		return request(Patch, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }), json::stringify(doc));
	}

	BoolResponse BaseDiscordClient::deleteMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		return { request(Delete, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID })), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::bulkDeleteMessages(Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messageIDs) {
		std::string JSON = "{\"messages\":[";
		for (Snowflake<Message> messageID : messageIDs) {
			JSON += messageID;
			JSON += ',';
		}
		if (messageIDs.size())
			JSON.pop_back();
		JSON += "]}";
		return { request(Post, path("channels/{channel.id}/messages/bulk-delete", { channelID }), JSON), EmptyRespFn() };
	}

	ObjectResponse<Channel> BaseDiscordClient::editChannel(Snowflake<Channel> channelID, std::string name, std::string topic) {
		std::string json = "{";
		if (name != "")
			json += "\"name\":\"" + name + "\",";
		if (topic != "")
			json += "\"topic\":\"" + topic + "\",";
		json[json.length() - 1] = '}';
		return request(Patch, path("channels/{channel.id}", { channelID }), json);
	}

	ObjectResponse<Channel> BaseDiscordClient::editChannelName(Snowflake<Channel> channelID, std::string name) {
		return editChannel(channelID, name);
	}

	ObjectResponse<Channel> BaseDiscordClient::editChannelTopic(Snowflake<Channel> channelID, std::string topic) {
		return editChannel(channelID, "", topic);
	}

	ObjectResponse<Channel> BaseDiscordClient::deleteChannel(Snowflake<Channel> channelID) {
		return request(Delete, path("channels/{channel.id}", { channelID }));
	}

	ObjectResponse<Channel> BaseDiscordClient::getChannel(Snowflake<Channel> channelID, RequestSettings<ObjectResponse<Channel>> settings) {
		return request(Get, path("channels/{channel.id}", { channelID }), settings);
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
		return request(Get,
			path("channels/{channel.id}/messages{key}{limit}", { channelID, key,
			(trueLimit != 0 ? "limit=" + std::to_string(trueLimit) : "")}));
	}

	ObjectResponse<Message> BaseDiscordClient::getMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		return request(Get, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }));
	}

	BoolResponse BaseDiscordClient::addReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji) {
		return { request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/@me", { channelID, messageID, emoji })), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::removeReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, Snowflake<User> userID) {
		return { request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/{user.id}", { channelID, messageID, emoji, userID })), EmptyRespFn() };
	}

	ArrayResponse<Reaction> BaseDiscordClient::getReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji) {
		return request(Get, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}", { channelID, messageID, emoji }));
	}

	StandardResponse BaseDiscordClient::removeAllReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		return request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions", { channelID, messageID }));
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

	ArrayResponse<Invite> BaseDiscordClient::getChannelInvites(Snowflake<Channel> channelID) {
		return request(Get, path("channels/{channel.id}/invites", { channelID }));
	}

	ObjectResponse<Invite> BaseDiscordClient::createChannelInvite(Snowflake<Channel> channelID, const uint64_t maxAge, const uint64_t maxUses, const bool temporary, const bool unique) {
		return request(Post, path("channels/{channel.id}/invites", { channelID }),
			json::createJSON({
				{"max_age", json::optionalUInteger(maxAge) },
				{"max_uses", json::optionalUInteger(maxUses) },
				{ "temporary", temporary ? "true" : ""},
				{ "unique", unique ? "true" : ""}
			}));
	}

	BoolResponse BaseDiscordClient::removeChannelPermission(Snowflake<Channel> channelID, std::string ID) {
		return { request(Delete, path("channels/{channel.id}/permissions/{overwrite.id}", { channelID, ID })), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::sendTyping(Snowflake<Channel> channelID) {
		return { request(Post, path("channels/{channel.id}/typing", { channelID })), EmptyRespFn() };
	}

	ArrayResponse<Message> BaseDiscordClient::getPinnedMessages(Snowflake<Channel> channelID) {
		return request(Get, path("channels/{channel.id}/pins", { channelID }));
	}

	BoolResponse BaseDiscordClient::pinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		return { request(Put, path("channels/{channel.id}/pins/{message.id}", { channelID, messageID })), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::unpinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		return { request(Delete, path("channels/{channel.id}/pins/{message.id}", { channelID, messageID })), EmptyRespFn() };
	}

	StandardResponse BaseDiscordClient::addRecipient(Snowflake<Channel> channelID, Snowflake<User> userID) {
		return request(Put, path("channels/{channel.id}/recipients/{user.id}", { channelID, userID }));
	}

	StandardResponse BaseDiscordClient::removeRecipient(Snowflake<Channel> channelID, Snowflake<User> userID) {
		return request(Delete, path("channels/{channel.id}/recipients/{user.id}", { channelID, userID }));
	}

	//
	//server functions
	//
	ObjectResponse<Channel> BaseDiscordClient::createTextChannel(Snowflake<Server> serverID, std::string name) {
		return request(Post, path("guilds/{guild.id}/channels", { serverID }), "{\"name\": \"" + name + "\", \"type\": \"text\"}");
	}

	ArrayResponse<Channel> BaseDiscordClient::editChannelPositions(Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions) {
		return request(Patch, path("guilds/{guild.id}/channels", { serverID }), getEditPositionString(positions));
	}

	ObjectResponse<ServerMember> SleepyDiscord::BaseDiscordClient::getMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return request(Get, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }));
	}

	ArrayResponse<ServerMember> BaseDiscordClient::listMembers(Snowflake<Server> serverID, uint16_t limit, std::string after) {
		//Todo better Query String Params support
		std::string limitParm = limit != 0 ? "?limit=" + std::to_string(limit) : "";
		std::string afterParm = after != "" ? "after=" + after : "";
		if (afterParm != "" && limitParm != "") limitParm += '&';
		else if (afterParm != "" && limitParm == "") limitParm += '?';
		return request(Get, path("guilds/{guild.id}/members{limit}{after}", { serverID, limitParm, afterParm }));
	}

	ObjectResponse<ServerMember> BaseDiscordClient::addMember(Snowflake<Server> serverID, Snowflake<User> userID, std::string accesToken, std::string nick, std::vector<Role> roles, bool mute, bool deaf) {
		std::string rolesString = "";
		if (roles.empty()){
			rolesString = "";
		} else {
			unsigned int i = -1;
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

		return request(Put, path("guilds/{guild.id}/members/{user.id}"), json::createJSON({
			{ "access_token", json::string (accesToken) },
			{ "nick"        , json::string (nick)       },
			{ "roles"       , rolesString               },
			{ "mute"        , json::boolean(mute)       },
			{ "deaf"        , json::boolean(deaf)       }
		}));
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

	ArrayResponse<Role> BaseDiscordClient::editRolePosition(Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions) {
		return request(Patch, path("guilds/{guild.id}/roles", { serverID }), getEditPositionString(positions));
	}


	StringResponse BaseDiscordClient::editRole(Snowflake<Server> serverID, Snowflake<Role> roleID, std::string name, Permission permissions, uint32_t color, int8_t hoist, int8_t mentionable) {
		const std::string colorString       = color       >> 24 == 0 ? std::to_string(color      ) : "";	//if over 24 bits, do not change color
		const std::string hoistString       = hoist       >> 1  == 0 ? json::boolean (hoist      ) : "";	//if larger then 1 bit, do change hoist
		const std::string mentionableString = mentionable >> 1  == 0 ? json::boolean (mentionable) : "";

		return request(Patch, path("guilds/{guild.id}/roles/{role.id}", { serverID, roleID }), json::createJSON({
			{ "name"       , json::string(name)         },
			{ "permissions", json::integer(permissions) },
			{ "color"      , colorString                },
			{ "hoist"      , hoistString                },
			{ "mentionable", mentionableString          }
		}));
	}

	BoolResponse SleepyDiscord::BaseDiscordClient::deleteRole(Snowflake<Server> serverID, Snowflake<Role> roleID) {
		return { request(Delete, path("guilds/{guild.id}/roles/{role.id}", { serverID, roleID })), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::muteServerMember(Snowflake<Server> serverID, Snowflake<User> userID, bool mute) {
		return { request(Patch, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), mute ? "{\"mute\":true}" : "{\"mute\":false}"), EmptyRespFn() };
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

	ObjectResponse<Server> BaseDiscordClient::getServer(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}", { serverID }));
	}

	ObjectResponse<Server> BaseDiscordClient::deleteServer(Snowflake<Server> serverID) {
		return request(Delete, path("guilds/{guild.id}", { serverID }));
	}

	ArrayResponse<Channel> SleepyDiscord::BaseDiscordClient::getServerChannels(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/channels", { serverID }));
	}

	BoolResponse BaseDiscordClient::editNickname(Snowflake<Server> serverID, std::string newNickname) {
		return { request(Patch, path("guilds/{guild.id}/members/@me/nick", { serverID }), "{\"nick\":\"" + newNickname + "\"}"), StandardRespFn() };
	}

	BoolResponse BaseDiscordClient::addRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID) {
		return { request(Put, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", { serverID, userID, roleID })), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::removeRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID) {
		return { request(Delete, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", { serverID, userID, roleID })), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::kickMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return { request(Delete, path("guilds/{guild.id}/members/{user.id}", { serverID, userID })), EmptyRespFn() };
	}

	ArrayResponse<User> BaseDiscordClient::getBans(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/bans", { serverID }));
	}

	BoolResponse BaseDiscordClient::banMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return { request(Put, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID })), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::unbanMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return { request(Delete, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID })), EmptyRespFn() };
	}

	ArrayResponse<Role> BaseDiscordClient::getRoles(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/roles", { serverID }));
	}

	ObjectResponse<Role> BaseDiscordClient::createRole(Snowflake<Server> serverID, std::string name, Permission permissions, unsigned int color, bool hoist, bool mentiionable) {
		return request(Post, path("guilds/{guild.id}/roles", { serverID }), json::createJSON({
			{ "name"       , json::string (name        ) },
			{ "permissions", json::integer(permissions ) },
			{ "color"      , json::integer(color       ) },
			{ "hoist"      , json::boolean(hoist       ) },
			{ "mentionable", json::boolean(mentiionable) }
		}));
	}

	StandardResponse BaseDiscordClient::pruneMembers(Snowflake<Server> serverID, const unsigned int numOfDays) {
		if (numOfDays == 0) return BAD_REQUEST;
		return request(Post, path("guilds/{guild.id}/prune", { serverID }), "{\"days\":" + std::to_string(numOfDays) + '}');
	}

	ArrayResponse<VoiceRegion> BaseDiscordClient::getVoiceRegions() {
		return request(Get, path("guilds/{guild.id}/regions"));
	}

	ArrayResponse<Invite> BaseDiscordClient::getServerInvites(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/invites", { serverID }));
	}

	StringResponse BaseDiscordClient::getIntegrations(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/integrations", { serverID }));
	}

	BoolResponse BaseDiscordClient::createIntegration(Snowflake<Server> serverID, std::string type, std::string integrationID) {
		return { request(Post, path("guilds/{guild.id}/integrations", { serverID }), json::createJSON({
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

	BoolResponse BaseDiscordClient::deleteIntegration(Snowflake<Server> serverID, std::string integrationID) {
		return { request(Delete, path("guilds/{guild.id}/integrations/{integration.id}", { serverID, integrationID })), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::syncIntegration(Snowflake<Server> serverID, std::string integrationID) {
		return { request(Post, path("guilds/{guild.id}/integrations/{integration.id}/sync", { serverID, integrationID })), EmptyRespFn() };
	}

	ObjectResponse<ServerEmbed> BaseDiscordClient::getServerEmbed(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/embed", { serverID }));
	}

	//
	//Invite functions
	//
	ObjectResponse<Invite> BaseDiscordClient::inviteEndpoint(RequestMethod method, std::string inviteCode) {
		return request(method, path("invites/{invite.code}", { inviteCode }));
	}

	ObjectResponse<Invite> BaseDiscordClient::getInvite(std::string inviteCode) {
		return inviteEndpoint(Get, inviteCode);
	}

	ObjectResponse<Invite> BaseDiscordClient::deleteInvite(std::string inviteCode) {
		return inviteEndpoint(Delete, inviteCode);
	}

	ObjectResponse<Invite> BaseDiscordClient::acceptInvite(std::string inviteCode) {
		return inviteEndpoint(Post, inviteCode);
	}
	//
	//User functions
	//
	ObjectResponse<User> BaseDiscordClient::getCurrentUser() {
		return request(Get, "users/@me");
	}

	ObjectResponse<User> BaseDiscordClient::getUser(Snowflake<User> userID) {
		return request(Get, path("users/{user.id}", { userID }));
	}

	ArrayResponse<Server> BaseDiscordClient::getServers() {
		return request(Get, "users/@me/guilds");
	}

	BoolResponse BaseDiscordClient::leaveServer(Snowflake<Server> serverID) {
		return { request(Delete, path("users/@me/guilds/{guild.id}", { serverID })), EmptyRespFn() };
	}

	ArrayResponse<Channel> BaseDiscordClient::getDirectMessageChannels() {
		return request(Get, "users/@me/channels");
	}

	ObjectResponse<Channel> BaseDiscordClient::createDirectMessageChannel(std::string recipientID) {
		return request(Post, "users/@me/channels", json::createJSON({ { "recipient_id", recipientID } }));
	}

	ArrayResponse<Connection> BaseDiscordClient::getUserConnections() {
		return request(Get, "users/@me/connections");
	}

	//
	//Webhook functions
	//
	ObjectResponse<Webhook> BaseDiscordClient::createWebhook(Snowflake<Channel> channelID, std::string name, std::string avatar) {
		return request(Post, path("channels/{channel.id}/webhooks", { channelID }), json::createJSON({
			{"name", json::string(name)},
			{"avatar", avatar}
		}));
	}

	ArrayResponse<Webhook> BaseDiscordClient::getChannelWebhooks(Snowflake<Channel> channelID) {
		return request(Get, path("channels/{channel.id}/webhooks", { channelID }));
	}

	ArrayResponse<Webhook> BaseDiscordClient::getServerWebhooks(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/webhooks", { serverID }));
	}

	inline const char* optionalWebhookToken(std::string webhookToken) {
		return webhookToken != "" ? "webhooks/{webhook.id}/{webhook.token}" : "webhooks/{webhook.id}";
	}

	ObjectResponse<Webhook> BaseDiscordClient::getWebhook(Snowflake<Webhook> webhookID, std::string webhookToken) {
		return request(Get, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }));
	}

	ObjectResponse<Webhook> BaseDiscordClient::editWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::string name, std::string avatar) {
		return request(Patch, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }), json::createJSON({
			{ "name", json::string(name) },
			{ "avatar", json::string(avatar) }
		}));
	}

	BoolResponse BaseDiscordClient::deleteWebhook(Snowflake<Webhook> webhookID, std::string webhookToken) {
		return { request(Delete, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken })), EmptyRespFn() };
	}

	//excute webhook

	ObjectResponse<Webhook> BaseDiscordClient::requestExecuteWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::pair<std::string, std::string> pair, bool wait, std::string username, std::string avatar_url, bool tts) {
		return request(Post, path("webhooks/{webhook.id}/{webhook.token}{wait}", { webhookID, webhookToken, (wait ? "?around=true" : "") }), json::createJSON({
			pair,
			{ "username"  , json::string(username  ) },
			{ "avatar_url", json::string(avatar_url) },
			{ "tts"       , (tts ? "true" : "")      }
		}));
	}

	ObjectResponse<Webhook> BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::string content, bool wait, std::string username, std::string avatar_url, bool tts) {
		return requestExecuteWebhook(webhookID, webhookToken, { "content", json::string(content) }, wait, username, avatar_url, tts);
	}

	//Webhook SleepyDiscord::BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::vector<Embed> embeds, bool wait, std::string username, std::string avatar_url bool tts) {
	//	
	//	return requestExecuteWebhook(webhookID, webhookToken, { "embeds", crazy stuff happens here }, wait, username, avatar_url, tts);
	//}
	
	ObjectResponse<Webhook> BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, filePathPart file, bool wait, std::string username, std::string avatar_url, bool tts) {
		return request(Post, path("webhooks/{webhook.id}/{webhook.token}", { webhookID, webhookToken }), {
			{ "file"      , filePathPart(file)  },
			{ "username"  , username            },
			{ "avatar_url", avatar_url          },
			{ "tts"       , (tts ? "true" : "") }
		});
	}
}