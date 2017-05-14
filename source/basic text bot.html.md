---
title: How to make a Basic Text Bot

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---
[< Go back to documentation](documentation.html)

#How to make a Basic Text Bot

<aside class="notice">
For this tutorial, I'm going to assume that you everything set up. If not, please read the quick start guild if it exist (it doesn't as of writing).
</aside>

## Including Sleepy Discord
>Use this for you want use your own input, output, and websocket

```cpp
#include "sleepy_discord/sleepy_discord.h"
```
As of this writing, to include Sleepy Discord you use the first one.
However this gives you very basic functionally with no websocket, input, or output. Sleepy Discord will call for these and it will expect that they will be done without failure. If you need more info about this, check out (future topic that I haven't written as of this writing)

>Use this for input and output from the c++ standard library and your websocket library of choice

```cpp
#include "sleepy_discord/websocketpp_websocket.h"
```
```cpp
#include "sleepy_discord/uwebsockets_websocket.h"
```
I'm going to assume that most people will not need to do this, and having all of this done for you is going to work fine for your proposes.
if so, the last two are what you are looking for, they include basic functionally for input, and output from the standard c++ library, and your websocket library of choice.

## Declaring the Client
Included in both ``websocketpp_websocket.h`` and ``uwebsockets_websocket.h`` is DiscordClient. This where almost everything that has to do with Discord is done.
By default, DiscordClient from ``websocketpp_websocket.h`` or ``uwebsockets_websocket.h`` will not do anything when it receives a message. This is where Events come into play.
When DiscordClient receives a message, it'll call a function that is an empty function by default. You must overwrite it with your own code to make it do something when DiscordClient receives a message.

>Use Inheritance to make a derived class with DiscordClient as the base class

```cpp
class myClientClass : public SleepyDiscord::DiscordClient {
	public:
	using SleepyDiscord::DiscordClient::DiscordClient;  //this should call the DiscordClient constructor
};
```
To do so you will need to create a new class from the existing DiscordClient class, inheriting all the properties and behavior of DiscordClient.

>redefine [onMessage](documentation.html#onmessage)

```cpp
class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	//redefining onMessage
	void onMessage(std::string* jsonMessage) {

	}
};
```
Now we need to redefine onMessage, this is what DiscordClient calls when it receives a message from Discord. This is where your own code will execute when DiscordClient receives a message.

## Initializing the Client
>Create a client

```cpp
int main() {
	myClientClass client("token");
}
```
The client can't do anything before it is initialized, all you have to do to initialize is creating a client like so. Oh, by the way, you'll need your [bot's token from here](https://discordapp.com/developers/applications/me).

<aside class="success">
 Also, don't forget that you'll need to prevent your bot from closing itself after the main function, unless you are only using one thread (which you probably aren't).
</aside>

## Make the bot respond to messages
>Create a message from the message

```cpp
void onMessage(std::string* jsonMessage) {
	SleepyDiscord::Message message(jsonMessage);
}
```
With that out of the way, onMessage takes in a pointer to a string as a parameter, this is the raw JSON object that the Discord API gives us. So, you create a Message from the JSON.
<aside class="notice">
Some of you maybe asking, "why doesn't it just do that for me"? It's because it's a planned feature that I never got around to adding in.
I'm sorry Danny.
</aside>

>Say Hello when someone says ``whcg hello``

```cpp
void onMessage(std::string* jsonMessage) {
	SleepyDiscord::Message message(jsonMessage);
	//say hello back when someone says hello to you
	if (message.startsWith("whcg hello"))
		sendMessage(message.channel_id, "Hello " + message.author.username);
}
```

Lets make our bot say hello and the person's name when someone says hello to the bot. This done by using the ``sendMessage`` function, and to get the person's name is stored in ``author.username``.
<aside class="warning">
Do not use ``if (message.startsWith("hello"))`` as it is not good practice. Your bot should not activate on normal chat.
</aside>
<aside class="success">
You may also use ``if (message.isMentioned(bot id))``
</aside>

##The code
>The finished code

```cpp
#include <thread>
#include <cstdio>
#include "websocketpp_websocket.h"

class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(std::string* jsonMessage) {
		SleepyDiscord::Message message(jsonMessage);
		if (message.startsWith("whcg hello"))
			sendMessage(message.channel_id, "Hello " + message.author.username);
	}
};

int main() {
	myClientClass client("token");
	std::getchar();	//wait for input to prevent the app from closing
}
```

By now this is what you should have.

##Testing Time
Now just compile it and just say ``whcg hello`` in any channel that your bot can access.
>Possible Output

```shell
Hello Sleepy Flower Girl
```

##Conclusion
Now that you understand the basics of using the Sleepy Discord Library, I'm sure that you can be able to read the rest of [the documentation](documentation.html) whenever you need any help. If you have any more questions, you can always ask Sleepy Flower Girl on Discord.