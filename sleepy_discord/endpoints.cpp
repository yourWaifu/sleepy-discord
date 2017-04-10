#include "client.h"

namespace SleepyDiscord {
	void DiscordClient::testFunction(std::string teststring) {
		disconnect(1001, "");
		connect(theGateway);
	}
	//
	//channel functions
	//
	Message DiscordClient::sendMessage(std::string channel_id, std::string message, bool tts) {
		auto r = request(Post, path("channels/{channel.id}/messages", channel_id), "{\"content\":\"" + message + (tts ? "\",\"tts\":\"true\"" : "\"") + "}");
		return Message(&r.text);
	}

	Message DiscordClient::uploadFile(std::string channel_id, std::string fileLocation, std::string message) {
		auto r = request(Post, "channels/" + channel_id + "/messages",
			cpr::Multipart{ { "content", message },
			{ "file", cpr::File{ fileLocation } }
		}
		);
		return Message(&r.text);
	}

	Message DiscordClient::editMessage(std::string channel_id, std::string message_id, std::string newMessage) {
		auto r = request(Patch, path("channels/{channel.id}/messages/{message.id}", channel_id, message_id), "{\"content\": \"" + newMessage + "\"}");
		return Message(&r.text);
	}

	bool DiscordClient::deleteMessage(const std::string channel_id, const std::string * message_id, const unsigned int numOfMessages) {
		if (numOfMessages == 1) return request(Delete, path("channels/{channel.id}/messages/{message.id}", channel_id, *message_id)).status_code == NO_CONTENT;
		else {
			std::string JSON = "{\"messages\":[";
			for (unsigned int i = 0; i < numOfMessages; ++i) {
				JSON += message_id[i];
			}
			JSON += "]}";
			return request(Delete, path("channels/{channel.id}/messages/{message.id}", channel_id, *message_id)).status_code == NO_CONTENT;
		}
	}

	Channel DiscordClient::editChannel(std::string channel_id, std::string name, std::string topic) {
		std::string json = "{";
		if (name != "")
			json += "\"name\":\"" + name + "\",";
		if (topic != "")
			json += "\"topic\":\"" + topic + "\",";
		json[json.length() - 1] = '}';
		auto r = request(Patch, path("channels/{channel.id}", channel_id), json);
		return Channel(&r.text);
	}

	Channel DiscordClient::editChannelName(std::string channel_id, std::string name) {
		return editChannel(channel_id, name);
	}

	Channel DiscordClient::editChannelTopic(std::string channel_id, std::string topic) {
		return editChannel(channel_id, "", topic);
	}

	Channel DiscordClient::deleteChannel(std::string channel_id) {
		auto r = request(Delete, path("channels/{channel.id}", channel_id));
		return Channel(&r.text);
	}

	Channel DiscordClient::getChannel(std::string channel_id) {
		std::string r = request(Get, path("channels/{channel.id}", channel_id)).text;
		return Channel(&r);
	}

	Message DiscordClient::getMessage(std::string channel_id, std::string message_id) {
		std::string r = request(Get, path("channels/{channel.id}/messages/{message.id}", channel_id, message_id)).text;
		return Message(&r);
	}

	bool DiscordClient::addReaction(std::string channel_id, std::string message_id, std::string emoji) {
		return request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/@me", channel_id, message_id, emoji)).status_code == NO_CONTENT;
	}

	bool DiscordClient::removeReaction(std::string channel_id, std::string message_id, std::string emoji, std::string user_id) {
		return request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/{user.id}", channel_id, message_id, emoji, user_id)).status_code == NO_CONTENT;
	}

	bool DiscordClient::sendTyping(std::string channel_id) {
		return request(Post, path("channels/{channel.id}/typing", channel_id)).status_code == NO_CONTENT;
	}

	bool DiscordClient::pinMessage(std::string channel_id, std::string message_id) {
		return request(Put, path("channels/{channel.id}/pins/{message.id}", channel_id, message_id)).status_code == NO_CONTENT;
	}

	bool DiscordClient::unpinMessage(std::string channel_id, std::string message_id) {
		return request(Delete, path("channels/{channel.id}/pins/{message.id}", channel_id, message_id)).status_code == NO_CONTENT;
	}
	//
	//server functions
	//
	Channel DiscordClient::createTextChannel(std::string server_id, std::string name) {
		auto r = request(Post, path("guilds/{guild.id}/channels", server_id), "{\"name\": \"" + name + "\", \"type\": \"text\"}");
		return Channel(&r.text);
	}

	int SleepyDiscord::DiscordClient::createRole(std::string server_id) {
		auto r = request(Post, path("guilds/{guild.id}/roles", server_id));
		return r.status_code;
	}

	bool SleepyDiscord::DiscordClient::deleteRole(std::string server_id, std::string role_id) {
		return request(Delete, path("guilds/{guild.id}/roles/{role.id}", server_id, role_id)).status_code == NO_CONTENT;
	}

	bool DiscordClient::muteServerMember(std::string server_id, std::string user_id, bool mute) {
		return request(Patch, path("guilds/{guild.id}/members/{user.id}", server_id, user_id), mute ? "{\"mute\":true}" : "{\"mute\":false}").status_code == NO_CONTENT;
	}

	Server DiscordClient::getServer(std::string server_id) {
		auto r = request(Get, path("guilds/{guild.id}", server_id));
		return Server(&r.text);
	}

	Server DiscordClient::deleteServer(std::string server_id) {
		auto r = request(Delete, path("guilds/{guild.id}", server_id));
		return Server(&r.text);
	}

	bool DiscordClient::editNickname(std::string server_id, std::string newNickname) {
		return request(Patch, path("guilds/{guild.id}/members/@me/nick", server_id), "{\"nick\":\"" + newNickname + "\"}").status_code == OK;
	}

	bool DiscordClient::addRole(std::string server_id, std::string member_id, std::string role_id) {
		return request(Put, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", server_id, member_id, role_id)).status_code == NO_CONTENT;
	}

	bool DiscordClient::removeRole(std::string server_id, std::string member_id, std::string role_id) {
		return request(Delete, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", server_id, member_id, role_id)).status_code == NO_CONTENT;
	}

	bool DiscordClient::kickMember(std::string server_id, std::string member_id) {
		return request(Delete, path("guilds/{guild.id}/members/{user.id}", server_id, member_id)).status_code == NO_CONTENT;
	}

	bool DiscordClient::banMember(std::string server_id, std::string member_id) {
		return request(Put, path("guilds/{guild.id}/bans/{user.id}", server_id, member_id)).status_code == NO_CONTENT;
	}

	bool DiscordClient::unbanMember(std::string server_id, std::string member_id) {
		return request(Delete, path("guilds/{guild.id}/bans/{user.id}", server_id, member_id)).status_code == NO_CONTENT;
	}

	void DiscordClient::pruneMembers(std::string server_id, const unsigned int numOfDays) {
		if (numOfDays == 0) return;
		request(Post, path("guilds/{guild.id}/prune", server_id), "{\"days\":" + numOfDays + '}');
	}

	bool DiscordClient::deleteIntegration(std::string server_id, std::string integration_id) {
		return request(Delete, path("guilds/{guild.id}/integrations/{integration.id}", server_id, integration_id)).status_code == NO_CONTENT;
	}

	bool DiscordClient::syncIntegration(std::string server_id, std::string integration_id) {
		return request(Post, path("guilds/{guild.id}/integrations/{integration.id}/sync", server_id, integration_id)).status_code == NO_CONTENT;
	}
	//
	//Invite functions
	//
	Invite DiscordClient::inviteEndpoint(RequestMethod method, std::string inviteCode) {
		auto r = request(method, path("invites/{invite.code}", inviteCode));
		return Invite(&r.text);
	}

	Invite DiscordClient::getInvite(std::string inviteCode) {
		return inviteEndpoint(Get, inviteCode);
	}

	Invite DiscordClient::deleteInvite(std::string inviteCode) {
		return inviteEndpoint(Delete, inviteCode);
	}

	Invite DiscordClient::acceptInvite(std::string inviteCode) {
		return inviteEndpoint(Post, inviteCode);
	}
	//
	//User functions
	//
	User DiscordClient::getCurrentUser() {
		auto r = request(Get, "users/@me");
		return User(&r.text);
	}

	User DiscordClient::getUser(std::string user_id) {
		auto r = request(Get, path("users/{user.id}", user_id));
		return User(&r.text);
	}

	bool DiscordClient::leaveServer(std::string server_id) {
		return request(Delete, path("users/@me/guilds/{guild.id}", server_id)).status_code == NO_CONTENT;
	}
}