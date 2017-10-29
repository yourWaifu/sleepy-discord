# Sleepy Discord
C++ library for Discord

Note: Sleepy Discord is pronounced as CP Discord, like the letters C and P. Actually, that's a joke. Sleepy should be pronounced the way you would normally pronounce sleepy.

# [Documentation](https://yourWaifu.github.io/sleepy-discord/documentation.html)
[Here is the link to the docs:](https://yourWaifu.github.io/sleepy-discord/documentation.html)
https://yourWaifu.github.io/sleepy-discord/documentation.html

If you like to edit them, please check out the docs branch.

# Why?
Just for the novelty of using a C++ library for Discord. I would also love for this thing to work on many things like consoles and maybe some microcontrollers.

# Example
```cpp
#include "sleepy_discord/websocketpp_websocket.h"

class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) {
		if (message.startsWith("whcg hello"))
			sendMessage(message.channel_id, "Hello " + message.author.username);
	}
};

int main() {
	myClientClass client("token", 2);
	client.run();
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

More complex examples:
 - [Rock Paper Scissors](https://github.com/yourWaifu/sleepy-discord/blob/master/examples/rock-paper-scissors/main.cpp)
 - [Unofficial Discord 3DS Client](https://github.com/yourWaifu/Unofficial-Discord-3DS-Client)
# Will Updating the library break my bot?

Yes, and for now I don't plan on making 0.0 versions backwards compatible with 1.0 versions or later.

# Requirements
Sleepy Discord doesn't require you to use any libraries, so that you can use any library you wish or your own code. However, Sleepy Discord provides native support for a few libraries and it is recommend you use those instead of writing your own.

* [OpenSSL](https://www.openssl.org/)
* [cpr](https://github.com/whoshuu/cpr)
* [Websocket++](https://github.com/zaphoyd/websocketpp)
or
[uWebSockets](https://github.com/uWebSockets/uWebSockets)

# Develop Branch
For the cutting edge of Sleepy Discord, check out the develop branch. Please use the develop branch with caution because it may not even be able to compile or it is 100% not tested at all. Other then that, the branch is used for code that may not work. Also check to see if it's not behind the master branch, unless you want to use an older version of Sleepy Discord.
