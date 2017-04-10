#pragma once
#include "client.h"

namespace SleepyDiscord {
	void BaseDiscordClient::onReady(std::string* jsonMessage) {

	}

	void BaseDiscordClient::onMessage(std::string* jsonMessage) {
		
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

	void BaseDiscordClient::onServer(std::string* jsonMessage) {
	
	}

	void BaseDiscordClient::onChannel(std::string* jsonMessage) {

	}

	void BaseDiscordClient::onEditedRole(std::string* jsonMessage) {

	}

	void BaseDiscordClient::onInvaldSession() {

	}

	void BaseDiscordClient::onDisconnet() {

	}
	
	void BaseDiscordClient::tick(float deltaTime) {
	
	}
	
	void BaseDiscordClient::onError(ErrorCode errorCode, std::string errorMessage) {
		if (errorCode != 0)
			std::cout << "Error " << errorCode << ": " + errorMessage + '\n';
		else
			std::cout << "Error " << ": " + errorMessage + '\n';
	}
}