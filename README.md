# Sleepy Discord
C++ library for Discord

Note: Sleepy Discord is pronounced as CP Discord, like the letters C and P. Actually, that's a joke. Sleepy should be pronounced the way you would normally pronounce sleepy.

[![Discord Server](https://discordapp.com/api/guilds/566681036935790615/embed.png?style=banner2)](https://discord.gg/5VDrVfK)

# [Documentation](https://yourWaifu.github.io/sleepy-discord/documentation.html)
[:pencil:](https://github.com/yourWaifu/sleepy-discord/edit/docs/source/setup.html.md "Edit Documentation") [Getting Started](https://yourwaifu.dev/sleepy-discord/setup.html)<br />
[:pencil:](https://github.com/yourWaifu/sleepy-discord/edit/docs/source/basic%20text%20bot.html.md "Edit Documentation") [How to make a Basic Text Bot](https://yourwaifu.github.io/sleepy-discord/basic%20text%20bot.html)<br />

If you like to edit them, [please check out the docs branch](https://github.com/yourWaifu/sleepy-discord/tree/docs) or click on the [:pencil:](https://github.com/yourWaifu/sleepy-discord/edit/docs/source/documentation.html.md "Edit Documentation").

## Build Status
| OS | Windows | Ubuntu
| ------ | ------- | ------
| Master   | [![Build Status](https://dev.azure.com/wuhao64/sleepy-discord/_apis/build/status/yourWaifu.sleepy-discord?branchName=master)](https://dev.azure.com/wuhao64/sleepy-discord/_build/?definitionId=2) | [![Build Status](https://travis-ci.org/yourWaifu/sleepy-discord.svg?branch=master)](https://travis-ci.org/yourWaifu/sleepy-discord)
| Develop   | [![Build Status](https://dev.azure.com/wuhao64/sleepy-discord/_apis/build/status/yourWaifu.sleepy-discord?branchName=develop)](https://dev.azure.com/wuhao64/sleepy-discord/_build/latest?definitionId=2) | [![Build Status](https://travis-ci.org/yourWaifu/sleepy-discord.svg?branch=develop)](https://travis-ci.org/yourWaifu/sleepy-discord)

# Why?
Just for the novelty of using a C++ library for Discord. I would also love for this thing to work on many things like consoles and maybe some microcontrollers.

# Example
```cpp
#include "sleepy_discord/sleepy_discord.h"

class MyClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override {
		if (message.startsWith("whcg hello"))
			sendMessage(message.channelID, "Hello " + message.author.username);
	}
};

int main() {
	myClientClass client("token", SleepyDiscord::USER_CONTROLED_THREADS);
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
 - [WebAssembly Client](https://github.com/yourWaifu/sleepy-discord/tree/master/examples/wasm_example) (Work In Progress)
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
