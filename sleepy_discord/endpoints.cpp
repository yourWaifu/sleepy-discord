#include "client.h"
#include "discord_object_interface.h"
#include "vector"

namespace SleepyDiscord {
	//
	//channel functions
	//
	ObjectResponse<Message> BaseDiscordClient::sendMessage(Snowflake<Channel> channelID, std::string message, bool tts) {
		return request(Post, path("channels/{channel.id}/messages", { channelID }), "{\"content\":\"" + message + (tts ? "\",\"tts\":\"true\"" : "\"") + "}");
	}

	ObjectResponse<Message> BaseDiscordClient::uploadFile(Snowflake<Channel> channelID, std::string fileLocation, std::string message) {
		return request(Post, path("channels/{channel.id}/messages", { channelID }),
			{ { "content", message },
			{ "file", filePathPart{fileLocation} }
		}
		);
	}

	ObjectResponse<Message> BaseDiscordClient::editMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string newMessage) {
		return request(Patch, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }), "{\"content\": \"" + newMessage + "\"}");
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

	ObjectResponse<Channel> BaseDiscordClient::getChannel(Snowflake<Channel> channelID) {
		return request(Get, path("channels/{channel.id}", { channelID }));
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

	bool BaseDiscordClient::addReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji) {
		return request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/@me", { channelID, messageID, emoji })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::removeReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, Snowflake<User> userID) {
		return request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/{user.id}", { channelID, messageID, emoji, userID })).statusCode == NO_CONTENT;
	}

	ArrayResponse<Reaction> BaseDiscordClient::getReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji) {
		return request(Get, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}", { channelID, messageID, emoji }));
	}

	void BaseDiscordClient::removeAllReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID) {
		request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions", { channelID, messageID }));
	}

	bool BaseDiscordClient::editChannelPermissions(Snowflake<Channel> channelID, Snowflake<Overwrite> overwriteID, int allow, int deny, std::string type) {
		return request(
			Put,
			path("channels/{channel.id}/permissions/{overwrite.id}", { channelID, overwriteID }),
			json::createJSON({
				{ "allow", std::to_string(allow) },
				{ "deny", std::to_string(deny) },
				{ "type", json::string(type) }
		})).statusCode == NO_CONTENT;
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

	bool BaseDiscordClient::removeChannelPermission(Snowflake<Channel> channelID, std::string ID) {
		return request(Delete, path("channels/{channel.id}/permissions/{overwrite.id}", { channelID, ID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::sendTyping(Snowflake<Channel> channelID) {
		return request(Post, path("channels/{channel.id}/typing", { channelID })).statusCode == NO_CONTENT;
	}

	ArrayResponse<Message> BaseDiscordClient::getPinnedMessages(Snowflake<Channel> channelID) {
		return request(Get, path("channels/{channel.id}/pins", { channelID }));
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
					{ "mentionable", json::boolean (role.mantionable) }
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

	ArrayResponse<Role> BaseDiscordClient::editRolePosition(Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions) {
		return request(Patch, path("guilds/{guild.id}/roles", { serverID }), getEditPositionString(positions));
	}


	std::string BaseDiscordClient::editRole(Snowflake<Server> serverID, Snowflake<Role> roleID, std::string name, Permission permissions, uint32_t color, int8_t hoist, int8_t mentionable) {
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

	ObjectResponse<Server> BaseDiscordClient::getServer(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}", { serverID }));
	}

	ObjectResponse<Server> BaseDiscordClient::deleteServer(Snowflake<Server> serverID) {
		return request(Delete, path("guilds/{guild.id}", { serverID }));
	}

	ArrayResponse<Channel> SleepyDiscord::BaseDiscordClient::getServerChannels(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/channels", { serverID }));
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

	ArrayResponse<User> BaseDiscordClient::getBans(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/bans", { serverID }));
	}

	bool BaseDiscordClient::banMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return request(Put, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID })).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::unbanMember(Snowflake<Server> serverID, Snowflake<User> userID) {
		return request(Delete, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID })).statusCode == NO_CONTENT;
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

	void BaseDiscordClient::pruneMembers(Snowflake<Server> serverID, const unsigned int numOfDays) {
		if (numOfDays == 0) return;
		request(Post, path("guilds/{guild.id}/prune", { serverID }), "{\"days\":" + numOfDays + '}');
	}

	ArrayResponse<VoiceRegion> BaseDiscordClient::getVoiceRegions() {
		return request(Get, path("guilds/{guild.id}/regions"));
	}

	ArrayResponse<Invite> BaseDiscordClient::getServerInvites(Snowflake<Server> serverID) {
		return request(Get, path("guilds/{guild.id}/invites", { serverID }));
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

	bool BaseDiscordClient::leaveServer(Snowflake<Server> serverID) {
		return request(Delete, path("users/@me/guilds/{guild.id}", { serverID })).statusCode == NO_CONTENT;
	}

	ArrayResponse<DMChannel> BaseDiscordClient::getDirectMessageChannels() {
		return request(Get, "users/@me/channels");
	}

	ObjectResponse<DMChannel> BaseDiscordClient::createDirectMessageChannel(std::string recipientID) {
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

	bool BaseDiscordClient::deleteWebhook(Snowflake<Webhook> webhookID, std::string webhookToken) {
		return request(Delete, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken })).statusCode == NO_CONTENT;
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