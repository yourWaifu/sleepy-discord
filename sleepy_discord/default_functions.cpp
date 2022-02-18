#include "client.h"

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning( disable: 4100 ) //warns about unused parameters with names
#pragma warning( disable: 4458 ) //warns about variables that hide class members
#endif

namespace SleepyDiscord {
	void BaseDiscordClient::onReady(Ready readyData) {

	}

	void BaseDiscordClient::onResumed() {

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

	void BaseDiscordClient::onMemberChunk(ServerMembersChunk memberChunk) {

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


	void BaseDiscordClient::onEditUserSettings(const json::Value& jsonMessage) {

	}

	void BaseDiscordClient::onEditVoiceState(VoiceState& state) {

	}

	void BaseDiscordClient::onTyping(Snowflake<Channel> channelID, Snowflake<User> userID, time_t timestamp) {

	}

	void BaseDiscordClient::onDeleteMessages(Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messages) {

	}

	void BaseDiscordClient::onEditMessage(MessageRevisions revisioins) {

	}

	void BaseDiscordClient::onEditVoiceServer(VoiceServerUpdate& voiceServerUpdate) {

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

	void BaseDiscordClient::onUnknownEvent(std::string name, const json::Value& data) {

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

	Timer BaseDiscordClient::schedule(TimedTask code, const time_t millisecondsTilDueTime) {
		return Timer([]() {});
	}
}