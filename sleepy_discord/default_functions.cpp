#include "client.h"

namespace SleepyDiscord {
	void BaseDiscordClient::onReady(std::string* jsonMessage) {

	}

	void BaseDiscordClient::onResumed(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onDeleteServer(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditServer(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onBan(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onUnban(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onMember(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onRemoveMember(std::string * jsonMessage) {

	}
	
	void BaseDiscordClient::onDeleteMember(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditMember(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onRole(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onDeleteRole(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditRole(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditEmojis(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onMemberChunk(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onDeleteChannel(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditChannel(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onPinMessage(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onPresenceUpdate(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditUser(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditUserNote(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditUserSettings(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditVoiceState(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onTyping(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onDeleteMessage(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditMessage(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onBulkDelete(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onEditVoiceServer(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onServerSync(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onRelationship(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onRemoveRelationship(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onDeleteRelationship(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onReaction(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onRemoveReaction(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onDeleteReaction(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onRemoveAllReaction(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onDeleteAllReaction(std::string * jsonMessage) {

	}

	void BaseDiscordClient::onMessage(Message message) {
		
	}

	void BaseDiscordClient::onEditedMessage(std::string* jsonMessage) {
		//onMessage(jsonMessage); 
		//because of the fact that on edited messages it'll contain only a subset of the full message object payload
		//this is will crash the client
	}

	void BaseDiscordClient::onHeartbeat() {

	}

	void BaseDiscordClient::onHeartbeatAck() {

	}

	void BaseDiscordClient::onServer(Server jsonMessage) {
	
	}

	void BaseDiscordClient::onChannel(std::string* jsonMessage) {

	}

	void BaseDiscordClient::onEditedRole(std::string* jsonMessage) {

	}

	void BaseDiscordClient::onDispatch(std::string * jsonMessage) {

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