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
		//for now keep this empty
	}

	void DiscordClient::onHeartbeat() {

	}

	void DiscordClient::onGiuld(JSON * jsonMessage) {
	
	}
	
	void DiscordClient::tick(float deltaTime) {
	
	}
	
	void DiscordClient::onError(int errorCode) {
	
	}
}