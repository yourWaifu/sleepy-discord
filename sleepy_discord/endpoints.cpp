#include "client.h"
#include "discord_object_interface.h"

namespace SleepyDiscord {
	void BaseDiscordClient::testFunction(std::string teststring) {
		/*disconnect(1001, "");
		connect(theGateway);*/
		getServerMember("202917641101246465", "99259409045143552");
	}
	//
	//channel functions
	//
	Message BaseDiscordClient::sendMessage(std::string channel_id, std::string message, bool tts) {
		return request<Message>(Post, path("channels/{channel.id}/messages", channel_id), "{\"content\":\"" + message + (tts ? "\",\"tts\":\"true\"" : "\"") + "}");
	}

	/*Message BaseDiscordClient::uploadFile(std::string channel_id, std::string fileLocation, std::string message) {
		auto r = request(Post, "channels/" + channel_id + "/messages",
			cpr::Multipart{ { "content", message },
			{ "file", cpr::File{ fileLocation } }
		}
		);
		return Message(&r.text);
	}*/

	Message BaseDiscordClient::editMessage(std::string channel_id, std::string message_id, std::string newMessage) {
		return request<Message>(Patch, path("channels/{channel.id}/messages/{message.id}", channel_id, message_id), "{\"content\": \"" + newMessage + "\"}");
	}

	bool BaseDiscordClient::deleteMessage(const std::string channel_id, const std::string * message_id, const unsigned int numOfMessages) {
		if (numOfMessages == 1) return request(Delete, path("channels/{channel.id}/messages/{message.id}", channel_id, *message_id)).statusCode == NO_CONTENT;
		std::string JSON = "{\"messages\":[";
		for (unsigned int i = 0; i < numOfMessages; ++i) {
			JSON += message_id[i];
		}
		JSON += "]}";
		return request(Post, path("channels/{channel.id}/messages/bulk-delete", channel_id), JSON).statusCode == NO_CONTENT;
	}

	Channel BaseDiscordClient::editChannel(std::string channel_id, std::string name, std::string topic) {
		std::string json = "{";
		if (name != "")
			json += "\"name\":\"" + name + "\",";
		if (topic != "")
			json += "\"topic\":\"" + topic + "\",";
		json[json.length() - 1] = '}';
		auto r = request(Patch, path("channels/{channel.id}", channel_id), json);
		return Channel(&r.text);
	}

	Channel BaseDiscordClient::editChannelName(std::string channel_id, std::string name) {
		return editChannel(channel_id, name);
	}

	Channel BaseDiscordClient::editChannelTopic(std::string channel_id, std::string topic) {
		return editChannel(channel_id, "", topic);
	}

	Channel BaseDiscordClient::deleteChannel(std::string channel_id) {
		return request<Channel>(Delete, path("channels/{channel.id}", channel_id));
	}

	Channel BaseDiscordClient::getChannel(std::string channel_id) {
		return request<Channel>(Get, path("channels/{channel.id}", channel_id));
	}

	std::vector<Message> BaseDiscordClient::getMessages(std::string channel_id, GetMessagesKey when, std::string message_id, uint8_t limit) {
		uint8_t trueLimit = 100 < limit ? 100 : limit;
		std::string key;
		switch (when) {
		case around: key = "?around=" + message_id; break;
		case before: key = "?before=" + message_id; break;
		case after: key = "?after=" + message_id; break;
		}
		return requestVector<Message>(Get, 
			path("/channels/{channel.id}/messages", channel_id) + key + (limit != 0 ? "&limit=" + std::to_string(limit) : ""));
	}

	Message BaseDiscordClient::getMessage(std::string channel_id, std::string message_id) {
		return request<Message>(Get, path("channels/{channel.id}/messages/{message.id}", channel_id, message_id));
	}

	bool BaseDiscordClient::addReaction(std::string channel_id, std::string message_id, std::string emoji) {
		return request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/@me", channel_id, message_id, emoji)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::removeReaction(std::string channel_id, std::string message_id, std::string emoji, std::string user_id) {
		return request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/{user.id}", channel_id, message_id, emoji, user_id)).statusCode == NO_CONTENT;
	}

	std::vector<Reaction> BaseDiscordClient::getReactions(std::string channel_id, std::string message_id, std::string emoji) {
		return requestVector<Reaction>(Get, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}", channel_id, message_id, emoji));
	}

	void BaseDiscordClient::removeAllReactions(std::string channel_id, std::string message_id) {
		request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions", channel_id, message_id));
	}

	bool BaseDiscordClient::editChannelPermissions(std::string channel_id, std::string id, int allow, int deny, std::string type) {
		return request(
			Put,
			path("channels/{channel.id}/permissions/{overwrite.id}", channel_id, id),
			json::createJSON({
				{ "allow", std::to_string(allow) },
				{ "deny", std::to_string(deny) },
				{ "type", json::string(type) }
		})).statusCode == NO_CONTENT;
	}

	std::vector<Invite> BaseDiscordClient::getChannelInvites(std::string channel_id) {
		return requestVector<Invite>(Get, path("channels/{channel.id}/invites", channel_id));
	}

	Invite BaseDiscordClient::createChannelInvite(std::string channel_id, const uint64_t maxAge, const uint64_t maxUses, const bool temporary, const bool unique) {
		return request<Invite>(Post, path("channels/{channel.id}/invites"),
			json::createJSON({
				{"max_age", json::optionalUInteger(maxAge) },
				{"max_uses", json::optionalUInteger(maxUses) },
				{ "temporary", temporary ? "true" : ""},
				{ "unique", unique ? "true" : ""}
			}));
	}

	bool BaseDiscordClient::removeChannelPermission(std::string channel_id, std::string id) {
		return request(Delete, path("channels/{channel.id}/permissions/{overwrite.id}", channel_id, id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::sendTyping(std::string channel_id) {
		return request(Post, path("channels/{channel.id}/typing", channel_id)).statusCode == NO_CONTENT;
	}

	std::vector<Message> BaseDiscordClient::getPinnedMessages(std::string channel_id) {
		return requestVector<Message>(Get, path("channels/{channel.id}/pins", channel_id));
	}

	bool BaseDiscordClient::pinMessage(std::string channel_id, std::string message_id) {
		return request(Put, path("channels/{channel.id}/pins/{message.id}", channel_id, message_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::unpinMessage(std::string channel_id, std::string message_id) {
		return request(Delete, path("channels/{channel.id}/pins/{message.id}", channel_id, message_id)).statusCode == NO_CONTENT;
	}

	void BaseDiscordClient::addRecipient(std::string channel_id, std::string user_id) {
		request(Put, path("channels/{channel.id}/recipients/{user.id}", channel_id, user_id));
	}

	void BaseDiscordClient::removeRecipient(std::string channel_id, std::string user_id) {
		request(Delete, path("channels/{channel.id}/recipients/{user.id}", channel_id, user_id));
	}

	//
	//server functions
	//
	Channel BaseDiscordClient::createTextChannel(std::string server_id, std::string name) {
		return request<Channel>(Post, path("guilds/{guild.id}/channels", server_id), "{\"name\": \"" + name + "\", \"type\": \"text\"}");
	}

	void SleepyDiscord::BaseDiscordClient::getServerMember(std::string server_id, std::string user_id) {
		request(Get, path("guilds/{guild.id}/members/{user.id}", server_id, user_id));
	}

	Role SleepyDiscord::BaseDiscordClient::createRole(std::string server_id) {
		return request<Role>(Post, path("guilds/{guild.id}/roles", server_id));
	}

	bool SleepyDiscord::BaseDiscordClient::deleteRole(std::string server_id, std::string role_id) {
		return request(Delete, path("guilds/{guild.id}/roles/{role.id}", server_id, role_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::muteServerMember(std::string server_id, std::string user_id, bool mute) {
		return request(Patch, path("guilds/{guild.id}/members/{user.id}", server_id, user_id), mute ? "{\"mute\":true}" : "{\"mute\":false}").statusCode == NO_CONTENT;
	}

	Server BaseDiscordClient::getServer(std::string server_id) {
		return request<Server>(Get, path("guilds/{guild.id}", server_id));
	}

	Server BaseDiscordClient::deleteServer(std::string server_id) {
		return request<Server>(Delete, path("guilds/{guild.id}", server_id));
	}

	std::vector<Channel> SleepyDiscord::BaseDiscordClient::GetServerChannels(std::string server_id) {
		return requestVector<Channel>(Get, path("guilds/{guild.id}/channels", server_id));
	}

	bool BaseDiscordClient::editNickname(std::string server_id, std::string newNickname) {
		return request(Patch, path("guilds/{guild.id}/members/@me/nick", server_id), "{\"nick\":\"" + newNickname + "\"}").statusCode == OK;
	}

	bool BaseDiscordClient::addRole(std::string server_id, std::string member_id, std::string role_id) {
		return request(Put, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", server_id, member_id, role_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::removeRole(std::string server_id, std::string member_id, std::string role_id) {
		return request(Delete, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", server_id, member_id, role_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::kickMember(std::string server_id, std::string member_id) {
		return request(Delete, path("guilds/{guild.id}/members/{user.id}", server_id, member_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::banMember(std::string server_id, std::string member_id) {
		return request(Put, path("guilds/{guild.id}/bans/{user.id}", server_id, member_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::unbanMember(std::string server_id, std::string member_id) {
		return request(Delete, path("guilds/{guild.id}/bans/{user.id}", server_id, member_id)).statusCode == NO_CONTENT;
	}

	void BaseDiscordClient::pruneMembers(std::string server_id, const unsigned int numOfDays) {
		if (numOfDays == 0) return;
		request(Post, path("guilds/{guild.id}/prune", server_id), "{\"days\":" + numOfDays + '}');
	}

	bool BaseDiscordClient::deleteIntegration(std::string server_id, std::string integration_id) {
		return request(Delete, path("guilds/{guild.id}/integrations/{integration.id}", server_id, integration_id)).statusCode == NO_CONTENT;
	}

	bool BaseDiscordClient::syncIntegration(std::string server_id, std::string integration_id) {
		return request(Post, path("guilds/{guild.id}/integrations/{integration.id}/sync", server_id, integration_id)).statusCode == NO_CONTENT;
	}
	//
	//Invite functions
	//
	Invite BaseDiscordClient::inviteEndpoint(RequestMethod method, std::string inviteCode) {
		return request<Invite>(method, path("invites/{invite.code}", inviteCode));
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

	User BaseDiscordClient::getUser(std::string user_id) {
		return request<User>(Get, path("users/{user.id}", user_id));
	}

	bool BaseDiscordClient::leaveServer(std::string server_id) {
		return request(Delete, path("users/@me/guilds/{guild.id}", server_id)).statusCode == NO_CONTENT;
	}
}