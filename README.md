# Sleepy Discord
Discord Library written in C++

# Why?
Just for the novelty of using a C++ library for Discord. I would also love for this thing to work on many things like consoles and maybe some microcontrollers.

# Example
```cpp
#include <sleepy_discord>

class myClientClass : public SleepyDiscord::DiscordClient {
public:
    using DiscordClient::DiscordClient;
    void onMessage(std::string * jsonMessage) {
   	    SleepyDiscord::Message message(jsonMessage);
	    if (message.content.find("whcg hello") == 0)
	        sendMessage(message.channel_id, "Hello " + message.author.username);
    }
};

int main() {
    myClientClass client("token");
    while(true)
        std::this_thread::sleep_for(std::chrono::seconds(1));
}
```
Input: Message received
```
whcg hello
```
Possible Output: Message sent
```
Hello Sleepy Flower Girl
```
# Will Updating the library break my bot?

Yes, and for now I don't plan on making 0.0 versions backwards compatable with 1.0 versions or later.

# Requirements
* [OpenSSL](https://www.openssl.org/)
* [Websocket++](https://github.com/zaphoyd/websocketpp)
* [cpr](https://github.com/whoshuu/cpr)
