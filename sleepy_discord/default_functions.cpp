#pragma once
#include "client.h"

namespace SleepyDiscord {
	void DiscordClient::onReady(JSON* jsonMessage) {

	}

	void DiscordClient::onMessage(JSON* jsonMessage) {

	}

	void DiscordClient::onEditedMessage(JSON * jsonMessage) {
		//onMessage(jsonMessage); 
		//because of the fact that on edited messages it'll contain only a subset of the full message object payload
		//this is will crash the client
	}

	void DiscordClient::onHeartbeat() {

	}

	void DiscordClient::onServer(JSON * jsonMessage) {
	
	}

	void DiscordClient::onChannel(JSON * jsonMessage) {

	}

	void DiscordClient::onEditedRole(JSON * jsonMessage) {

	}

	void DiscordClient::onDisconnet() {

	}
	
	void DiscordClient::tick(float deltaTime) {
	
	}
	
	void DiscordClient::onError(ErrorCode errorCode, std::string errorMessage) {
		std::cout << "Error " << errorCode << ": " + errorMessage + '\n';
	}
}