#include "sleepy_discord/sleepy_discord.h"

class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) {
		if (message.startsWith("whcg hello"))
			sendMessage(message.channelID, "Hello " + message.author.username);
	}
};

int main() {
	myClientClass client("token", SleepyDiscord::BaseDiscordClient::USER_CONTROLED_THREADS);
	client.run();
}
