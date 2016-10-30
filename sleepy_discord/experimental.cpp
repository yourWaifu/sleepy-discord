#include "experimental.h"
#include "client.h"

std::chrono::high_resolution_clock::time_point startTimer = std::chrono::high_resolution_clock::now();	//this is so that I can collect data from my experiments

namespace SleepyDiscord {
	Message DiscordClient::sendMessage(std::string channel_id, std::string message) {	//proof of concept
																						//if (MAX_MESSAGES_SENT_PER_MINUTE <= numOfMessagesSent) return 429;	//Error Code for too many request
		auto r = request(Post, "channels/" + channel_id + "/messages", "{\"content\": \"" + message + "\"}");
		//JSON* json = JSON_parseJSON(r.text.c_str(), r.text.length());
		//JSON_deallocate(json);
		startTimer = std::chrono::high_resolution_clock::now();
		return Message(&r.text);
	}

	void DiscordClient::onMessage(std::string* jsonMessage) {}
}
