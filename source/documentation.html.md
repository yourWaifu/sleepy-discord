---
title: Sleepy Discord Documentation

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---

# A Warm Welcome

Hello there, if you are looking for some help on using the Sleepy Discord Library, then you came to the right place (I hope it is). If you have any questions you can ask me ( Sleepy Flower Girl ) on discord; right now there is no official server for this library, but you can alway find me on the Discord api server. If you like to help, you can always make a pull request for the docs or the library itself on github. Thanks!

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

## deleteChannel

 ```cpp 
Channel deleteChannel(std::string channel_id);
```

### Parameters
<table>
  <tbody>
      <tr><td><strong>channel_id</strong></td>
        <td>The id of the channel that you delete</td></tr>
  </tbody>
</table>

### Return value
The channel was just deleted

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

<aside class="note">
If you want to send a new line, use ``\\n```
</aside>

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

## editMessage

```cpp 
Message editMessage(std::string channel_id, std::string message_id, std::string newMessage);
Message editMessage(Message message, std::string newMessage);
```

Edits an existing Message

### Parameters
<table>
  <tbody>
      <tr><td><strong>channel_id</strong></td>
        <td>The id of the channel with the message that you want to edit</td></tr>
      <tr><td><strong>message_id</strong></td>
        <td>The id of the message you want to edit</td></tr>
      <tr><td><strong>message</strong></td>
        <td>The message with the id of the message that you want to edit</td></tr>
      <tr><td><strong>newMessage</strong></td>
        <td>The new message that you want the message to be</td></tr>
  </tbody>
</table>

### Return value
Returns the message you just edited

## pinMessage

```cpp 
bool pinMessage(std::string channel_id, std::string message_id);
```

Puts a message into the Pinned Messages of a channel

### Parameters
<table>
  <tbody>
      <tr><td><strong>channel_id</strong></td>
        <td>The id of the channel with the message that you want to pin</td></tr>
      <tr><td><strong>message_id</strong></td>
        <td>The id of the message you want to pin</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## unpinMessage

```cpp 
bool unpinMessage(std::string channel_id, std::string message_id);
```

Removes a message from Pinned Messages of a channel

### Parameters
<table>
  <tbody>
      <tr><td><strong>channel_id</strong></td>
        <td>The id of the channel with the message that you want to unpin</td></tr>
      <tr><td><strong>message_id</strong></td>
        <td>The id of the message you want to unpin</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## deleteMessage

```cpp 
bool deleteMessage(const std::string channel_id, const std::string* message_id, const unsigned int numOfMessages = 1);
```

Deletes a message

### Parameters
<table>
  <tbody>
      <tr><td><strong>channel_id</strong></td>
        <td>The id of the channel with the messages that you want to delete</td></tr>
      <tr><td><strong>message_id</strong></td>
        <td>An array of ids of the messages you want to delete</td></tr>
      <tr><td><strong>numOfMessages</strong></td>
        <td>the number of messages to delete</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## editNickname

```cpp 
bool editNickname(std::string server_id, std::string newNickname);
```

Changes the name that is displayed on a server

### Parameters
<table>
  <tbody>
      <tr><td><strong>server_id</strong></td>
        <td>The id of the server where you want to this nickname</td></tr>
      <tr><td><strong>newNickname</strong></td>
        <td>The Nickname that you want your bot to have</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## addRole

```cpp 
bool addRole(std::string server_id, std::string member_id, std::string role_id);
```

Gives a member a role on a server 

### Parameters
<table>
  <tbody>
      <tr><td><strong>server_id</strong></td>
        <td>The id of the server with the user you want to give the role to</td></tr>
      <tr><td><strong>member_id</strong></td>
        <td>The id of the user that you want to give the role to</td></tr>
      <tr><td><strong>role_id</strong></td>
        <td>The id of role that you want to give</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## removeRole

```cpp 
bool removeRole(std::string server_id, std::string member_id, std::string role_id);
```

Takes away a role from a member on a server

### Parameters
<table>
  <tbody>
      <tr><td><strong>server_id</strong></td>
        <td>The id of the server with the user you want to remove the role from</td></tr>
      <tr><td><strong>member_id</strong></td>
        <td>The id of the user that you want to remove the role from</td></tr>
      <tr><td><strong>role_id</strong></td>
        <td>The id of role that you want to remove from the user</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## kickMember

```cpp 
bool kickMember(std::string server_id, std::string member_id);
```

Removes a member from a server

### Parameters
<table>
  <tbody>
      <tr><td><strong>server_id</strong></td>
        <td>The id of the server with the user you want to remove</td></tr>
      <tr><td><strong>member_id</strong></td>
        <td>The id of the user that you want to remove</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## updateStatus

```cpp 
void updateStatus(std::string gameName = "", uint64_t idleSince = 0);
```

Updates the bot's status

### Parameters
<table>
  <tbody>
      <tr><td><strong>gameName</strong></td>
        <td>The text right under the username, without the ``Playing``</td></tr>
      <tr><td><strong>idleSince</strong></td>
        <td>unix time in milliseconds when the client when idle, or null if the client is not idle</td></tr>
  </tbody>
</table>

## isReady

```cpp 
const bool isReady();
```

Tell you if the Ready event has ocurred or not

### Return value
``true`` if the ready event ocurred

## quit

```cpp 
void quit();
```

lets you disconnect from discord and stops the client

## run

```cpp 
virtual void run();
```

# DiscordClient Events

Events are functions that can be overridden that are called when an event such as receiving a message occur. For example, the function onMessage is an event.

## onReady

```cpp
virtual void onReady(std::string* jsonMessage);
```

###Parameters
<table>
  <tbody>
      <tr><td><strong>milliseconds</strong></td>
        <td>The amount of time to sleep for in milliseconds</td></tr>
  </tbody>
</table>

## onMessage

```cpp
virtual void onMessage(SleepyDiscord::Message message);
```
```cpp
#include <sleepy_discord>

class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message m) {
		if (m.startsWith("whcg hello")) {
			SleepyDiscord::Message message = sendMessage(message.channel_id, "Hello " + message.author.username);
			std::cout << message.content;
		}
	}
};

int main() {
	myClientClass client("token", 2);
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
      <tr><td><strong>message</strong></td>
        <td>Message object with all the info from the <a href="https://discordapp.com/developers/docs/resources/channel#message-object">MESSAGE_CREATE event</a></td></tr>
  </tbody>
</table>

## onQuit

```cpp
virtual void onQuit();
```

Happens when the client is quitting and after disconnecting from Discord

## sleep

```cpp
virtual void sleep(const unsigned int milliseconds);
```

###Parameters
<table>
  <tbody>
      <tr><td><strong>milliseconds</strong></td>
        <td>The amount of time to sleep for in milliseconds</td></tr>
  </tbody>
</table>

Called when the client needs to wait a bit

## onError

```cpp
virtual void onError(ErrorCode errorCode, const std::string errorMessage);
```

Happens when an error is detected

### Parameters
<table>
  <tbody>
      <tr><td><strong>errorCode</strong></td>
        <td>The number associated with the error</td></tr>
      <tr><td><strong>errorMessage</strong></td>
        <td>The reason or message for the given error code</td></tr>
  </tbody>
</table>

# Discord Objects

# Message

```cpp
struct Message : public DiscordObject {
  	public:
		Message();
		~Message();
		Message(const std::string * rawJson);
		Message(BaseDiscordClient* client, std::string channel_id, std::string message, bool tts = false);
```

An object that represents a message from Discord. Also very similar to [the message object from the api](https://discordapp.com/developers/docs/resources/channel#message-object)

## startsWith

```cpp
bool startsWith(char* test);
```

### Parameters
<table>
  <tbody>
      <tr><td><strong>test</strong></td>
        <td>the string that to compare the beginning of the message to</td></tr>
  </tbody>
</table>

### Return value
``ture`` when the message starts with the given string

## length

```cpp
int length();
```

### Return value
The length of the content in the message

## isMentioned

```cpp
bool isMentioned(const std::string id);
bool isMentioned(User& _user);
```

### Parameters
<table>
  <tbody>
      <tr><td><strong>id</strong></td>
        <td>the id of the user that you want to check that they were mentioned or not</td></tr>
      <tr><td><strong>_user</strong></td>
        <td>The user object of the user you want to check that they were mentioned or not </td></tr>
  </tbody>
</table>

### Return value
``ture`` if the user was mentioned

## send

```cpp
Message send(BaseDiscordClient * client);
```

###Parameters
<table>
  <tbody>
      <tr><td><strong>client</strong></td>
        <td>The client that will be used to send the message</td></tr>
  </tbody>
</table>

### Return value
The Message you just sent as a Message object

## reply

```cpp
Message reply(BaseDiscordClient * client, std::string message, bool tts = false);
```

sends a message to same channel that the parent message is on

### Parameters
<table>
  <tbody>
      <tr><td><strong>client</strong></td>
        <td>The client that will be used to send the reply</td></tr>
      <tr><td><strong>message</strong></td>
        <td>The message to use to reply</td></tr>
      <tr><td><strong>tts</strong></td>
        <td>Short for Text to Speech. When this is true, anyone with text to speech messages on will have their computer read the message out load to the them</td></tr>
  </tbody>
</table>

### Return value
The Message you just sent as a Message object

## operator==
```cpp
bool operator==(const std::string& message);
```

Compares the content of two messages (does not compare ids)

###Parameters
<table>
  <tbody>
      <tr><td><strong>message</strong></td>
        <td>The message that is being compared</td></tr>
  </tbody>
</table>

### Return value
``true`` when the content of the two messages are the same

# User
```cpp
struct User : public DiscordObject {
		~User();
		User();
		User(const std::string * rawJSON);
```

Based on [the object with the same name from the api](https://discordapp.com/developers/docs/resources/user#user-object)

## operator==

```cpp
bool operator==(const User& rightUser);
```

Compares the id of two Users

###Parameters
<table>
  <tbody>
      <tr><td><strong>rightUser</strong></td>
        <td>The User that is being compared</td></tr>
  </tbody>
</table>

### Return value
``ture`` when the two Users have the same id