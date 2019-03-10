#include "client.h"

namespace SleepyDiscord {
	void BaseDiscordClient::onReady(Ready readyData) {

	}

	void BaseDiscordClient::onResumed(const json::Value& jsonMessage) {

	}

	void BaseDiscordClient::onDeleteServer(UnavailableServer server) {

	}

	void BaseDiscordClient::onEditServer(Server server) {

	}

	void BaseDiscordClient::onBan(Snowflake<Server> serverID, User user) {

	}

	void BaseDiscordClient::onUnban(Snowflake<Server> serverID, User user) {

	}

	void BaseDiscordClient::onMember(Snowflake<Server> serverID, ServerMember member) {

	}

	void BaseDiscordClient::onRemoveMember(Snowflake<Server> serverID, User user) {

	}


	void BaseDiscordClient::onEditMember(Snowflake<Server> serverID, User user, std::vector<Snowflake<Role>> roles, std::string nick) {

	}

	void BaseDiscordClient::onRole(Snowflake<Server> serverID, Role role) {

	}

	void BaseDiscordClient::onDeleteRole(Snowflake<Server> serverID, Snowflake<Role> roleID) {

	}

	void BaseDiscordClient::onEditRole(Snowflake<Server> serverID, Role role) {

	}

	void BaseDiscordClient::onEditEmojis(Snowflake<Server> serverID, std::vector<Emoji> emojis) {

	}

	void BaseDiscordClient::onMemberChunk(Snowflake<Server> serverID, std::vector<ServerMember> members) {

	}

	void BaseDiscordClient::onDeleteChannel(Channel channel) {

	}

	void BaseDiscordClient::onEditChannel(Channel channel) {

	}

	void BaseDiscordClient::onPinMessage(Snowflake<Channel> channelID, std::string lastPinTimestamp) {

	}

	void BaseDiscordClient::onPresenceUpdate(PresenceUpdate presenseUpdate) {

	}

	void BaseDiscordClient::onEditUser(User user) {

	}

	void BaseDiscordClient::onEditUserNote(const json::Value& jsonMessage) {

	}

	void BaseDiscordClient::onEditUserSettings(const json::Value& jsonMessage) {

	}

	void BaseDiscordClient::onEditVoiceState(VoiceState& state) {

	}

	void BaseDiscordClient::onTyping(Snowflake<Channel> channelID, Snowflake<User> userID, time_t timestamp) {

	}

	void BaseDiscordClient::onDeleteMessages(Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messages) {

	}

	void BaseDiscordClient::onEditMessage(const json::Value& jsonMessage) {
		//because of the fact that on edited messages it'll contain only a subset of the full message object payload
		//We need a way to edit a message without editing the whole thing
	}

	void BaseDiscordClient::onEditVoiceServer(VoiceServerUpdate& voiceServerUpdate) {

	}

	void BaseDiscordClient::onServerSync(const json::Value& jsonMessage) {

	}

	void BaseDiscordClient::onRelationship(const json::Value& jsonMessage) {

	}

	void BaseDiscordClient::onDeleteRelationship(const json::Value& jsonMessage) {

	}

	void BaseDiscordClient::onReaction(Snowflake<User> userID, Snowflake<Channel> channelID, Snowflake<Message> messageID, Emoji emoji) {

	}

	void BaseDiscordClient::onDeleteReaction(Snowflake<User> userID, Snowflake<Channel> channelID, Snowflake<Message> messageID, Emoji emoji) {

	}

	void BaseDiscordClient::onDeleteAllReaction(Snowflake<Server> serverID, Snowflake<Channel> channelID, Snowflake<Message> messageID) {

	}

	void BaseDiscordClient::onMessage(Message message) {
		
	}

	void BaseDiscordClient::onHeartbeat() {

	}

	void BaseDiscordClient::onHeartbeatAck() {

	}

	void BaseDiscordClient::onServer(Server jsonMessage) {
	
	}

	void BaseDiscordClient::onChannel(Channel channel) {

	}

	void BaseDiscordClient::onDispatch(const json::Value& jsonMessage) {

	}

	void BaseDiscordClient::onInvaldSession() {

	}

	void BaseDiscordClient::onDisconnect() {

	}

	void BaseDiscordClient::onResume() {

	}

	void BaseDiscordClient::runAsync() {

	}

	void BaseDiscordClient::run() {

	}

	void BaseDiscordClient::onQuit() {

	}

	void SleepyDiscord::BaseDiscordClient::onResponse(Response response) {
	}

	void BaseDiscordClient::sleep(const unsigned int milliseconds) {
	
	}
	
	void BaseDiscordClient::fileRead(const char* path, std::string*const file) {

	}

	void BaseDiscordClient::tick(float deltaTime) {
	
	}
	
	void BaseDiscordClient::onError(ErrorCode errorCode, std::string errorMessage) {
		
	}
}