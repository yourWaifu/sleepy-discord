#pragma once
#include "client.h"

namespace SleepyDiscord {
	void DiscordClient::onReady(std::string* jsonMessage) {

	}

	void DiscordClient::onMessage(std::string* jsonMessage) {
		
	}

	void DiscordClient::onEditedMessage(std::string* jsonMessage) {
		//onMessage(jsonMessage); 
		//because of the fact that on edited messages it'll contain only a subset of the full message object payload
		//this is will crash the client
	}

	void DiscordClient::onHeartbeat() {

	}

	void DiscordClient::onServer(std::string* jsonMessage) {
	
	}

	void DiscordClient::onChannel(std::string* jsonMessage) {

	}

	void DiscordClient::onEditedRole(std::string* jsonMessage) {

	}

	void DiscordClient::onDisconnet() {

	}
	
	void DiscordClient::tick(float deltaTime) {
	
	}
	
	void DiscordClient::onError(ErrorCode errorCode, std::string errorMessage) {
		std::cout << "Error " << errorCode << ": " + errorMessage + '\n';
	}
}