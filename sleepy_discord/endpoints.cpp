#include "client.h"
#include "discord_object_interface.h"

namespace SleepyDiscord {
	void BaseDiscordClient::testFunction(std::string teststring) {
		//request(Post, path("guilds/{guild.id}/roles", std::string("202917641101246465")), json::createJSON({
		//		{"color", json::UInteger(0x1000000)}
		//}));
	}
	//
	//channel functions
	//
	ObjectResponse<Message> BaseDiscordClient::sendMessage(Snowflake<Channel> channelID, std::string message, bool tts) {
		return request(Post, path("channels/{channel.id}/messages", { channelID }), "{\"content\":\"" + message + (tts ? "\",\"tts\":\"true\"" : "\"") + "}");
	}

	Message BaseDiscordClient::uploadFile(Snowflake<Channel> channelID, std::string fileLocation, std::string message) {
		auto r = request(Post, "channels/" + channelID + "/messages",
			{ { "content", message },
			{ "file", filePathPart{fileLocation} }
		}
		);
		return Message(&r.text);
	}

	Message BaseDiscordClient::editMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string newMessage) {
		return request<Message>(Patch, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }), "{\"content\": \"" + newMessage + "\"}");
	}

	bool BaseDiscordClient::deleteMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		return request(Delete, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::bulkDeleteMessages(Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messageIDs) {
		std::string JSON = "{\"messages\":[";
		for (Snowflake<Message> messageID : messageIDs) {
			JSON += messageID;
			JSON += ',';
		}
		if (messageIDs.size())
			JSON.pop_back();
		JSON += "]}";
		return request(Post, path("channels/{channel.id}/messages/bulk-delete", { channelID }), JSON).statusCode == NO_CONTENT;
	}

	Channel BaseDiscordClient::editChannel(Snowflake<Channel> channelID, std::string name, std::string topic) {
		std::string json = "{";
		if (name != "")
			json += "\"name\":\"" + name + "\",";
		if (topic != "")
			json += "\"topic\":\"" + topic + "\",";
		json[json.length() - 1] = '}';
		auto r = request(Patch, path("channels/{channel.id}", { channelID }), json);
		return Channel(&r.text);
	}

	Channel BaseDiscordClient::editChannelName(Snowflake<Channel> channelID, std::string name) {
		return editChannel(channelID, name);
	}

	Channel BaseDiscordClient::editChannelTopic(Snowflake<Channel> channelID, std::string topic) {
		return editChannel(channelID, "", topic);
	}

	Channel BaseDiscordClient::deleteChannel(Snowflake<Channel> channelID) {
		return request<Channel>(Delete, path("channels/{channel.id}", { channelID }));
	}

	Channel BaseDiscordClient::getChannel(Snowflake<Channel> channelID) {
		return request<Channel>(Get, path("channels/{channel.id}", { channelID }));
	}

	json::ArrayWrapper<Message> BaseDiscordClient::getMessages(Snowflake<Channel> channelID, GetMessagesKey when, Snowflake<Message> messageID, uint8_t _limit) {
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
			(trueLimit != 0 ? "limit=" + std::to_string(trueLimit) : "")})).text;
	}

	Message BaseDiscordClient::getMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		return request<Message>(Get, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }));
	}

	bool BaseDiscordClient::addReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji) {
		return request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/@me", { channelID, messageID, emoji })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::removeReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, Snowflake<User> userID) {
		return request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/{user.id}", { channelID, messageID, emoji, userID })).statusCode == NO_CONTENT;
	}

	std::vector<Reaction> BaseDiscordClient::getReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji) {
		return requestVector<Reaction>(Get, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}", { channelID, messageID, emoji }));
	}

	void BaseDiscordClient::removeAllReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions", { channelID, messageID }));
	}

	bool BaseDiscordClient::editChannelPermissions(Snowflake<Channel> channelID, std::string ID, int allow, int deny, std::string type) {
		return request(
			Put,
			path("channels/{channel.id}/permissions/{overwrite.id}", { channelID, ID }),
			json::createJSON({
				{ "allow", std::to_string(allow) },
				{ "deny", std::to_string(deny) },
				{ "type", json::string(type) }
		})).statusCode == NO_CONTENT;
	}

	std::vector<Invite> BaseDiscordClient::getChannelInvites(Snowflake<Channel> channelID) {
		return requestVector<Invite>(Get, path("channels/{channel.id}/invites", { channelID }));
	}

	Invite BaseDiscordClient::createChannelInvite(Snowflake<Channel> channelID, const uint64_t maxAge, const uint64_t maxUses, const bool temporary, const bool unique) {
		return request<Invite>(Post, path("channels/{channel.id}/invites"), 
			json::createJSON({
				{"max_age", json::optionalUInteger(maxAge) },
				{"max_uses", json::optionalUInteger(maxUses) },
				{ "temporary", temporary ? "true" : ""},
				{ "unique", unique ? "true" : ""}
			}));
	}

	bool BaseDiscordClient::removeChannelPermission(Snowflake<Channel> channelID, std::string ID) {
		return request(Delete, path("channels/{channel.id}/permissions/{overwrite.id}", { channelID, ID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::sendTyping(Snowflake<Channel> channelID) {
		return request(Post, path("channels/{channel.id}/typing", { channelID })).statusCode == NO_CONTENT;
	}

	std::vector<Message> BaseDiscordClient::getPinnedMessages(Snowflake<Channel> channelID) {
		return requestVector<Message>(Get, path("channels/{channel.id}/pins", { channelID }));
	}

	bool BaseDiscordClient::pinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		return request(Put, path("channels/{channel.id}/pins/{message.id}", { channelID, messageID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::unpinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		return request(Delete, path("channels/{channel.id}/pins/{message.id}", { channelID, messageID })).statusCode == NO_CONTENT;
	}

	void BaseDiscordClient::addRecipient(Snowflake<Channel> channelID, Snowflake<User> userID) {
		request(Put, path("channels/{channel.id}/recipients/{user.id}", { channelID, userID }));
	}

	void BaseDiscordClient::removeRecipient(Snowflake<Channel> channelID, Snowflake<User> userID) {
		request(Delete, path("channels/{channel.id}/recipients/{user.id}", { channelID, userID }));
	}

	//
	//server functions
	//
	Channel BaseDiscordClient::createTextChannel(Snowflake<Server> serverID, std::string name) {
		return request<Channel>(Post, path("guilds/{guild.id}/channels", { serverID }), "{\"name\": \"" + name + "\", \"type\": \"text\"}");
	}

	std::vector<Channel> BaseDiscordClient::editChannelPositions(Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions) {
		return requestVector<Channel>(Patch, path("guilds/{guild.id}/channels", { serverID }), getEditPositionString(positions));
	}

	ServerMember SleepyDiscord::BaseDiscordClient::getMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return request<ServerMember>(Get, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }));
	}

	std::vector<ServerMember> BaseDiscordClient::listMembers(Snowflake<Server> serverID, uint16_t limit, std::string after) {
		return requestVector<ServerMember>(Get, path("guilds/{guild.id}/members", { serverID }), json::createJSON({
			{ "limit", json::optionalUInteger(limit) },
			{ "after", json::string(after) }
		}));
	}

	ServerMember BaseDiscordClient::addMember(Snowflake<Server> serverID, Snowflake<User> userID, std::string accesToken, std::string nick, std::vector<Role> roles, bool mute, bool deaf) {
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
					{ "mentionable", json::boolean (role.mantionable) }
				});
			}
			rolesString = json::createJSONArray(values);
		}

		return request<ServerMember>(Put, path("guilds/{guild.id}/members/{user.id}"), json::createJSON({
			{ "access_token", json::string (accesToken) },
			{ "nick"        , json::string (nick)       },
			{ "roles"       , rolesString               },
			{ "mute"        , json::boolean(mute)       },
			{ "deaf"        , json::boolean(deaf)       }
		}));
	}

	std::vector<Role> BaseDiscordClient::editRolePosition(Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions) {
		return requestVector<Role>(Patch, path("guilds/{guild.id}/roles", { serverID }), getEditPositionString(positions));
	}

	std::string BaseDiscordClient::editRole(Snowflake<Server> serverID, Snowflake<Role> roleID, std::string name, Permission permissions, uint32_t color, int position, uint8_t hoist, uint8_t mentionable) {
		const std::string colorString       = color       >> 24 == 0 ? std::to_string(color      ) : "";	//if over 24 bits, do not change color
		const std::string hoistString       = hoist       >> 1  == 0 ? json::boolean (hoist      ) : "";	//if larger then 1 bit, do change hoist
		const std::string mentionableString = mentionable >> 1  == 0 ? json::boolean (mentionable) : "";

		return request(Patch, path("guilds/{guild.id}/roles/{role.id}", { serverID, roleID }), json::createJSON({
			{ "name"       , json::string(name)         },
			{ "permissions", json::integer(permissions) },
			{ "color"      , colorString                },
			{ "hoist"      , hoistString                },
			{ "mentionable", mentionableString          }
		})).text;
	}

	bool SleepyDiscord::BaseDiscordClient::deleteRole(Snowflake<Server> serverID, Snowflake<Role> roleID) {
		return request(Delete, path("guilds/{guild.id}/roles/{role.id}", { serverID, roleID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::muteServerMember(Snowflake<Server> serverID, Snowflake<User> userID, bool mute) {
		return request(Patch, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), mute ? "{\"mute\":true}" : "{\"mute\":false}").statusCode == NO_CONTENT;
	}

	Server BaseDiscordClient::getServer(Snowflake<Server> serverID) {
		return request<Server>(Get, path("guilds/{guild.id}", { serverID }));
	}

	Server BaseDiscordClient::deleteServer(Snowflake<Server> serverID) {
		return request<Server>(Delete, path("guilds/{guild.id}", { serverID }));
	}

	json::ArrayWrapper<Channel> SleepyDiscord::BaseDiscordClient::GetServerChannels(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/channels", { serverID })).text;
	}

	bool BaseDiscordClient::editNickname(Snowflake<Server> serverID, std::string newNickname) {
		return request(Patch, path("guilds/{guild.id}/members/@me/nick", { serverID }), "{\"nick\":\"" + newNickname + "\"}").statusCode == OK;
	}

	bool BaseDiscordClient::addRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID) {
		return request(Put, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", { serverID, userID, roleID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::removeRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID) {
		return request(Delete, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", { serverID, userID, roleID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::kickMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return request(Delete, path("guilds/{guild.id}/members/{user.id}", { serverID, userID })).statusCode == NO_CONTENT;
	}

	std::vector<User> BaseDiscordClient::getBans(Snowflake<Server> serverID) {
		return requestVector<User>(Get, path("guilds/{guild.id}/bans", { serverID }));
	}

	bool BaseDiscordClient::banMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return request(Put, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::unbanMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return request(Delete, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID })).statusCode == NO_CONTENT;
	}

	std::vector<Role> BaseDiscordClient::getRoles(Snowflake<Server> serverID) {
		return requestVector<Role>(Get, path("guilds/{guild.id}/roles", { serverID }));
	}

	Role BaseDiscordClient::createRole(Snowflake<Server> serverID, std::string name, Permission permissions, unsigned int color, bool hoist, bool mentiionable) {
		return request<Role>(Post, path("guilds/{guild.id}/roles", { serverID }), json::createJSON({
			{ "name"       , json::string (name        ) },
			{ "permissions", json::integer(permissions ) },
			{ "color"      , json::integer(color       ) },
			{ "hoist"      , json::boolean(hoist       ) },
			{ "mentionable", json::boolean(mentiionable) }
		}));
	}

	void BaseDiscordClient::pruneMembers(Snowflake<Server> serverID, const unsigned int numOfDays) {
		if (numOfDays == 0) return;
		request(Post, path("guilds/{guild.id}/prune", { serverID }), "{\"days\":" + numOfDays + '}');
	}

	std::vector<VoiceRegion> BaseDiscordClient::getVoiceRegions() {
		return requestVector<VoiceRegion>(Get, path("guilds/{guild.id}/regions"));
	}

	std::vector<Invite> BaseDiscordClient::getServerInvites(Snowflake<Server> serverID) {
		return requestVector<Invite>(Get, path("guilds/{guild.id}/invites", { serverID }));
	}

	std::string BaseDiscordClient::getIntegrations(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/integrations", { serverID })).text;
	}

	bool BaseDiscordClient::createIntegration(Snowflake<Server> serverID, std::string type, std::string integrationID) {
		return request(Post, path("guilds/{guild.id}/integrations", { serverID }), json::createJSON({
			{ "type", json::string(type) },
			{ "id", json::string(integrationID) }
		})).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::editIntergration(Snowflake<Server> serverID, std::string integrationID, int expireBegavior, int expireGracePeriod, bool enbleEmoticons) {
		return request(Patch, path("guilds/{guild.id}/integrations/{integration.id}", { serverID, integrationID }), json::createJSON({
			{ "expire_behavior", json::integer(expireBegavior) },
			{ "expire_grace_period", json::integer(expireGracePeriod) },
			{ "enable_emoticons", json::boolean(enbleEmoticons) }
		})).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::deleteIntegration(Snowflake<Server> serverID, std::string integrationID) {
		return request(Delete, path("guilds/{guild.id}/integrations/{integration.id}", { serverID, integrationID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::syncIntegration(Snowflake<Server> serverID, std::string integrationID) {
		return request(Post, path("guilds/{guild.id}/integrations/{integration.id}/sync", { serverID, integrationID })).statusCode == NO_CONTENT;
	}
	ServerEmbed BaseDiscordClient::getServerEmbed(Snowflake<Server> serverID) {
		return request<ServerEmbed>(Get, path("guilds/{guild.id}/embed", { serverID }));
	}
	//
	//Invite functions
	//
	Invite BaseDiscordClient::inviteEndpoint(RequestMethod method, std::string inviteCode) {
		return request<Invite>(method, path("invites/{invite.code}", { inviteCode }));
	}

	Invite BaseDiscordClient::getInvite(std::string inviteCode) {
		return inviteEndpoint(Get, inviteCode);
	}

	Invite BaseDiscordClient::deleteInvite(std::string inviteCode) {
		return inviteEndpoint(Delete, inviteCode);
	}

	Invite BaseDiscordClient::acceptInvite(std::string inviteCode) {
		return inviteEndpoint(Post, inviteCode);
	}
	//
	//User functions
	//
	User BaseDiscordClient::getCurrentUser() {
		return request<User>(Get, "users/@me");
	}

	User BaseDiscordClient::getUser(Snowflake<User> userID) {
		return request<User>(Get, path("users/{user.id}", { userID }));
	}

	json::ArrayWrapper<Server> BaseDiscordClient::getServers() {
		return request(Get, "users/@me/guilds").text;
	}

	bool BaseDiscordClient::leaveServer(Snowflake<Server> serverID) {
		return request(Delete, path("users/@me/guilds/{guild.id}", { serverID })).statusCode == NO_CONTENT;
	}

	std::vector<DMChannel> BaseDiscordClient::getDirectMessageChannels() {
		return requestVector<DMChannel>(Get, "users/@me/channels");
	}

	DMChannel BaseDiscordClient::createDirectMessageChannel(std::string recipientID) {
		return request<DMChannel>(Post, "users/@me/channels", json::createJSON({ { "recipient_id", recipientID } }));
	}

	std::vector<Connection> BaseDiscordClient::getUserConnections() {
		return requestVector<Connection>(Get, "users/@me/connections");
	}

	//
	//Webhook functions
	//
	Webhook BaseDiscordClient::createWebhook(Snowflake<Channel> channelID, std::string name, std::string avatar) {
		return request<Webhook>(Post, path("channels/{channel.id}/webhooks", { channelID }), json::createJSON({
			{"name", json::string(name)},
			{"avatar", avatar}
		}));
	}

	std::vector<Webhook> BaseDiscordClient::getChannelWebhooks(Snowflake<Channel> channelID) {
		return requestVector<Webhook>(Get, path("channels/{channel.id}/webhooks", { channelID }));
	}

	std::vector<Webhook> BaseDiscordClient::getServerWebhooks(Snowflake<Server> serverID) {
		return requestVector<Webhook>(Get, path("guilds/{guild.id}/webhooks", { serverID }));
	}

	inline const char* optionalWebhookToken(std::string webhookToken) {
		return webhookToken != "" ? "webhooks/{webhook.id}/{webhook.token}" : "webhooks/{webhook.id}";
	}

	Webhook BaseDiscordClient::getWebhook(Snowflake<Webhook> webhookID, std::string webhookToken) {
		return request<Webhook>(Get, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }));
	}

	Webhook BaseDiscordClient::editWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::string name, std::string avatar) {
		return request<Webhook>(Patch, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }), json::createJSON({
			{ "name", json::string(name) },
			{ "avatar", json::string(avatar) }
		}));
	}

	bool BaseDiscordClient::deleteWebhook(Snowflake<Webhook> webhookID, std::string webhookToken) {
		return request(Delete, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken })).statusCode == NO_CONTENT;
	}

	//excute webhook

	Webhook BaseDiscordClient::requestExecuteWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::pair<std::string, std::string> pair, bool wait, std::string username, std::string avatar_url, bool tts) {
		return request<Webhook>(Post, path("webhooks/{webhook.id}/{webhook.token}{wait}", { webhookID, webhookToken, (wait ? "?around=true" : "") }), json::createJSON({
			pair,
			{ "username"  , json::string(username  ) },
			{ "avatar_url", json::string(avatar_url) },
			{ "tts"       , (tts ? "true" : "")      }
		}));
	}

	Webhook BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::string content, bool wait, std::string username, std::string avatar_url, bool tts) {
		return requestExecuteWebhook(webhookID, webhookToken, { "content", json::string(content) }, wait, username, avatar_url, tts);
	}

	//Webhook SleepyDiscord::BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::vector<Embed> embeds, bool wait, std::string username, std::string avatar_url bool tts) {
	//	
	//	return requestExecuteWebhook(webhookID, webhookToken, { "embeds", crazy stuff happens here }, wait, username, avatar_url, tts);
	//}
	
	Webhook BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, filePathPart file, bool wait, std::string username, std::string avatar_url, bool tts) {
		std::string response = request(Post, path("webhooks/{webhook.id}/{webhook.token}", { webhookID, webhookToken }), {
			{ "file"      , filePathPart(file)  },
			{ "username"  , username            },
			{ "avatar_url", avatar_url          },
			{ "tts"       , (tts ? "true" : "") }
		}).text;
		return Webhook(&response);
	}
}