#include "sleepy_discord.h"

class myClientClass : public SleepyDiscord::DiscordClient {
public:
	myClientClass(const std::string token) : DiscordClient(token) {}
	void onMessage(JSON* jsonMessage) {
		SleepyDiscord::Message message(this, jsonMessage);
		if (message.startsWith("whcg hello"))
			message.reply("Hello " + message.author.username);
	}
};

int main() {
	myClientClass client("token");
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}