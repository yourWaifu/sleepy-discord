#include "client.h"

namespace SleepyDiscord {
	void BaseDiscordClient::onReady(std::string *) {

	}

	void BaseDiscordClient::onResumed(std::string *) {

	}

	void BaseDiscordClient::onDeleteServer(std::string *) {

	}

	void BaseDiscordClient::onEditServer(std::string *) {

	}

	void BaseDiscordClient::onBan(std::string *) {

	}

	void BaseDiscordClient::onUnban(std::string *) {

	}

	void BaseDiscordClient::onMember(std::string *) {

	}

	void BaseDiscordClient::onRemoveMember(std::string *) {

	}
	
	void BaseDiscordClient::onDeleteMember(std::string *) {

	}

	void BaseDiscordClient::onEditMember(std::string *) {

	}

	void BaseDiscordClient::onRole(std::string *) {

	}

	void BaseDiscordClient::onDeleteRole(std::string *) {

	}

	void BaseDiscordClient::onEditRole(std::string *) {

	}

	void BaseDiscordClient::onEditEmojis(std::string *) {

	}

	void BaseDiscordClient::onMemberChunk(std::string *) {

	}

	void BaseDiscordClient::onDeleteChannel(std::string *) {

	}

	void BaseDiscordClient::onEditChannel(std::string *) {

	}

	void BaseDiscordClient::onPinMessage(std::string *) {

	}

	void BaseDiscordClient::onPresenceUpdate(std::string *) {

	}

	void BaseDiscordClient::onEditUser(std::string *) {

	}

	void BaseDiscordClient::onEditUserNote(std::string *) {

	}

	void BaseDiscordClient::onEditUserSettings(std::string *) {

	}

	void BaseDiscordClient::onEditVoiceState(std::string * ) {

	}

	void BaseDiscordClient::onTyping(std::string *) {

	}

	void BaseDiscordClient::onDeleteMessage(std::string *) {

	}

	void BaseDiscordClient::onEditMessage(std::string *) {

	}

	void BaseDiscordClient::onBulkDelete(std::string *) {

	}

	void BaseDiscordClient::onEditVoiceServer(std::string *) {

	}

	void BaseDiscordClient::onServerSync(std::string *) {

	}

	void BaseDiscordClient::onRelationship(std::string *) {

	}

	void BaseDiscordClient::onRemoveRelationship(std::string *) {

	}

	void BaseDiscordClient::onDeleteRelationship(std::string *) {

	}

	void BaseDiscordClient::onReaction(std::string *) {

	}

	void BaseDiscordClient::onRemoveReaction(std::string *) {

	}

	void BaseDiscordClient::onDeleteReaction(std::string *) {

	}

	void BaseDiscordClient::onRemoveAllReaction(std::string *) {

	}

	void BaseDiscordClient::onDeleteAllReaction(std::string *) {

	}

	void BaseDiscordClient::onMessage(Message) {
		
	}

	void BaseDiscordClient::onEditedMessage(std::string*) {
		//onMessage(jsonMessage); 
		//because of the fact that on edited messages it'll contain only a subset of the full message object payload
		//this is will crash the client
	}

	void BaseDiscordClient::onHeartbeat() {

	}

	void BaseDiscordClient::onHeartbeatAck() {

	}

	void BaseDiscordClient::onServer(Server) {
	
	}

	void BaseDiscordClient::onChannel(std::string *) {

	}

	void BaseDiscordClient::onEditedRole(std::string *) {

	}

	void BaseDiscordClient::onDispatch(std::string *) {

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

	void SleepyDiscord::BaseDiscordClient::onResponse(Response) {
	}

	void BaseDiscordClient::sleep(const unsigned int) {
	
	}
	
	void BaseDiscordClient::fileRead(const char*, std::string*const) {

	}

	void BaseDiscordClient::tick(float) {
	
	}
	
	void BaseDiscordClient::onError(ErrorCode, std::string) {
		
	}
}
