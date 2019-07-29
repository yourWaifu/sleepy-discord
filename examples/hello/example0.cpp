#include "sleepy_discord/sleepy_discord.h"

class MyClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override {
		if (message.startsWith("whcg hello"))
			sendMessage(message.channelID, "Hello " + message.author.username);
		else if (message.startsWith("whcg test"))
			testFunction("");
	}
};

int main() {
	MyClientClass client("NDczMzM5MjY2NTIxNjI4Njcz.XRu71Q.Fw1-3MvIeMZA49nzXX_d7sVaa14", SleepyDiscord::USER_CONTROLED_THREADS);
	client.run();
}
