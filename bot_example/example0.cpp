#include <thread>
#include <experimental.h>

class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using DiscordClient::DiscordClient;
	void onMessage(std::string* jsonMessage) {
		SleepyDiscord::Message message(jsonMessage);
		if (message.startsWith("whcg hello"))
			sendMessage(message.channel_id, "Hello " + message.author.username);
	}
};

int main() {
	myClientClass client("token");
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}