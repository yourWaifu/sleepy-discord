---
title: Sleepy Discord Documentation

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---

# A Warm Welcome

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Hello there, if you are looking for some help on using the Sleepy Discord Library, then you came to the right place (I hope it is). If you have any questions you can ask me ( Sleepy Flower Girl ) on discord; right now there is no official server for this library, but you can alway find me on the Discord api server. If you like to help, you can always make a pull request for the docs or the library itself on github. Thanks!

# Topics
[How To Compile Sleepy Discord](compile.html)

[How To Make a Basic Text Bot](basic text bot.html)

# DiscordClient

```cpp
class DiscordClient {
```
The DiscordClient class is the base class for a client that can be used to send and read messages.

[Declared in `client.h`](https://github.com/NoNamer64/sleepy-discord/blob/master/sleepy_discord/client.h)

# DiscordClient Functions

## sendMessage

 ```cpp 
Message sendMessage(std::string channel_id, std::string message, bool tts = false);
```
```cpp
#include <sleepy_discord.h>
#include <iostream>

int main() {
    myClientClass client("token");
    client.waitTilReady();
    SleepyDiscord::Message message = client.sendMessage("channel id", "Hello");
    std::cout << message.content;
}
```
>Output: Message sent

```shell
Hello
```

Post a message to a channel.

### Parameters
<table>
  <tbody>
      <tr><td><strong>channel_id</strong></td>
        <td>The id of the channel you want to post the message to</td></tr>
      <tr><td><strong>message</strong></td>
        <td>The message you want to post</td></tr>
      <tr><td><strong>tts</strong></td>
        <td>Short for Text to Speech. When this is true, anyone with text to speech messages on will have their computer read the message out load to the them</td></tr>
  </tbody>
</table>

### Return value
The Message you just sent as a Message object

### Other Details
[Declared in `client.h`](https://github.com/NoNamer64/sleepy-discord/blob/master/sleepy_discord/client.h) and [defined in `endpoints.cpp`](https://github.com/NoNamer64/sleepy-discord/blob/master/sleepy_discord/endpoints.cpp)

Uses [Create Message](https://discordapp.com/developers/docs/resources/channel#create-message)

## addReaction

 ```cpp 
bool SleepyDiscord::DiscordClient::addReaction(std::string channel_id, std::string message_id, std::string emoji);
```
```cpp
#include <sleepy_discord>

int main() {
	SleepyDiscord::DiscordClient client("token");
	client.waitTilReady();
	client.addReaction("channel id", "message id", "%F0%9F%98%95");
}
```
>Output: added 😕 reaction

Adds reaction to a message.

### Parameters
<table>
  <tbody>
    <tr>
      <td><strong>channel_id</strong></td>
      <td>The id of the channel with the message you want to add a reaction to</td>
    </tr>
    <tr>
      <td><strong>message_id</strong></td>
      <td>The id of the message you want to add a reaction to</td>
    </tr>
    <tr>
      <td><strong>emoji</strong></td>
      <td>The emoji you want to use for the reaction
        <ul>
          <li>Use <a href="https://en.wikipedia.org/wiki/Percent-encoding">Percent Encoding</a> for Unicode Emoji</li>
          <li>For custom emoji, use the id of the emoji (I haven't tested this myself)</li>
        </ul>
      </td>
    </tr>
  </tbody>
</table>

### Return value
true on success, otherwise false

### Other Details
[Declared in `client.h`](https://github.com/NoNamer64/sleepy-discord/blob/master/sleepy_discord/client.h) and [defined in `endpoints.cpp`](https://github.com/NoNamer64/sleepy-discord/blob/master/sleepy_discord/endpoints.cpp)

Uses [Create Reaction](https://discordapp.com/developers/docs/resources/channel#create-reaction)

# DiscordClient Events

Events are functions that can be overridden that are called when an event such as receiving a message occur. For example, the function onMessage is an event.

## onMessage

```cpp
virtual void onMessage(std::string* jsonMessage);
```
```cpp
#include <sleepy_discord>

class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using DiscordClient::DiscordClient;
	void onMessage(std::string* jsonMessage) {
		SleepyDiscord::Message message(jsonMessage);
		if (message.startsWith("whcg hello")) {
			SleepyDiscord::Message message = sendMessage(message.channel_id, "Hello " + message.author.username);
			std::cout << message.content;
		}
	}
};

int main() {
	myClientClass client("token");
	while(true)
		std::this_thread::sleep_for(std::chrono::seconds(1));
}
```
>Input: Message received

```shell
whcg hello
```
>Possible Output: Message sent

```shell
Hello Sleepy Flower Girl
```

Called when the Client receives a new message.

###Parameters
<table>
  <tbody>
      <tr><td><strong>jsonMessage</strong></td>
        <td>Parsed JSON with all the info from the <a href="https://discordapp.com/developers/docs/topics/gateway#message-create">MESSAGE_CREATE event</a></td></tr>
  </tbody>
</table>

# Discord Objects

## Message

```cpp
struct Message : public DiscordObject {
  	public:
		Message();
		~Message();
		Message(const std::string * rawJson);
		Message(BaseDiscordClient* client, std::string channel_id, std::string message, bool tts = false);
```

