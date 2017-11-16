---
title: How To Make a Basic Text Bot with Sleepy Discord

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---
[⮌ Go back to documentation](documentation.html)

#How to make a Basic Text Bot

<aside class="notice">
For this tutorial, I'm going to assume that you everything set up. If not, please read <a href="compile.html">How To Compile Sleepy Discord</a>.
</aside>

## Including Sleepy Discord
>Use this if you want to use your own input, output, and websocket

```cpp
#include "sleepy_discord/sleepy_discord.h"
```
As of this writing, to include Sleepy Discord you use ``sleepy_discord.h``.
However this gives you the bare minimum, no input, output or websockets. Sleepy Discord will call for these and it will expect that they will be done without failure. If you need more info about this, check out (future topic that I haven't written as of this writing)

>Use this for input and output from the c++ standard library and your websocket library of choice

```cpp
#include "sleepy_discord/websocketpp_websocket.h"
```
```cpp
#include "sleepy_discord/uwebsockets_websocket.h"
```
I'm going to assume that most people will not need to do this and having all of this done for you is going to work fine for your proposes.
If so, the last two are what you are looking for. They include basic functionally for input, and output from the standard C++ library, and your websocket library of choice.

## Declaring the Client
Included in both ``websocketpp_websocket.h`` and ``uwebsockets_websocket.h`` is DiscordClient. This where almost everything that has to do with Discord is completed.
By default, DiscordClient from ``websocketpp_websocket.h`` or ``uwebsockets_websocket.h`` will not do anything when it receives a message. This is where Events come into play.
When DiscordClient receives a message, it'll call an empty function by default. You must overwrite it with your own code to make it do something when DiscordClient receives a message.

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
	void onMessage(SleepyDiscord::Message message) {

	}
};
```
Now we need to redefine onMessage, this is what DiscordClient calls when it receives a message from Discord. This is where your own code will execute when DiscordClient receives a message.

## Initializing the Client
>Create a client

```cpp
int main() {
	myClientClass client("token", 2);
	client.run();
}
```
The client can't do anything before it is initialized, so you have to initialize a client like so (Oh, by the way, you'll need your [bot's token from here](https://discordapp.com/developers/applications/me)). Also take note of the number 2, that's the number of threads Sleepy Discord will want to use. Also, just in case you couldn't tell, Calling ``run`` will run Sleepy Discord.

<aside class="notice">
 If you changed threads to 3, you'll need something to prevent your bot from closing itself. Also, do not call ``run`` with 3 or more threads, because Sleepy Discord will call run on another thread.<br>
 1 thread is a planned feature coming Soon™.
</aside>

## Make the bot respond to messages
>Take another look at onMessage

```cpp
void onMessage(SleepyDiscord::Message message) {
	
}
```
With that out of the way, onMessage takes in a Message object that you can use to read messages that others have posted.
<aside class="notice">
For now all other events take in the raw JSON from Discord using ``std::string``. ``onMessage`` is currently the only event that doesn't give you the raw JSON.
</aside>

>Say Hello when someone says ``whcg hello``

```cpp
void onMessage(SleepyDiscord::Message message) {
	//say hello back when someone says hello to you
	if (message.startsWith("whcg hello"))
		sendMessage(message.channelID, "Hello " + message.author.username);
}
```

Lets make our bot say hello and the person's name when someone says hello to the bot. This done by using the ``sendMessage`` function, and to get the person's name is stored in ``author.username``.
<aside class="warning">
Do not use ``if (message.startsWith("hello"))`` it is not good practice. Your bot should not activate on normal chat.
</aside>
<aside class="success">
You may also use ``if (message.isMentioned(bot id))``
</aside>

##The code
>The finished code

```cpp
#include "websocketpp_websocket.h"

class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) {
		if (message.startsWith("whcg hello"))
			sendMessage(message.channelID, "Hello " + message.author.username);
	}
};

int main() {
	myClientClass client("token", 2);
	client.run();
}
```

By now this is what you should have.

##Testing Time
Now just compile it and run it. Now, just say ``whcg hello`` in any channel that your bot can access, and the bot will say hello back to you.
>Possible Output

```shell
Hello Sleepy Flower Girl
```

##Conclusion
Now that you understand the basics of using the Sleepy Discord Library, I'm sure that you can be able to read the rest of [the documentation](documentation.html) whenever you need help. If you have any more questions, you can always ask Sleepy Flower Girl on Discord.
