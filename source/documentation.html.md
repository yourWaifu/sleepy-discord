---
title: Sleepy Discord Documentation

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---

# A Warm Welcome

Hello there, if you are looking for some help on using the Sleepy Discord Library, then you came to the right place (I hope it is). If you have any questions you can ask me ( Sleepy Flower Girl ) on Discord; right now there is no official server for this library, but you can alway find me on the Discord API server. If you like to help, you can always make a pull request for the docs or the library itself on github. Thanks!

# Topics
<table>
  <tbody>
    <tr>
      <td><a href="compile.html">How To Compile Sleepy Discord</a></td>
      <td><a href="basic text bot.html">How To Make a Basic Text Bot</a></td>
    </tr>
    <tr>
      <td><a href="link.html">How To Link Sleepy Discord</a></td>
      <td><a>  </a></td>
    </tr>
  </tbody>
</table>

# DiscordClient

```cpp
class DiscordClient {
```
The DiscordClient class is the base class for a client that can be used to send and read messages.

[Declared in `client.h`](https://github.com/NoNamer64/sleepy-discord/blob/master/sleepy_discord/client.h)

# DiscordClient Functions

## deleteChannel

```cpp 
Channel deleteChannel(Snowflake<Channel> channelID);
```

### Parameters
<table>
  <tbody>
      <tr><td><strong>channelID</strong></td>
        <td>The id of the channel that you delete</td></tr>
  </tbody>
</table>

### Return value
The channel was just deleted

## sendMessage

 ```cpp 
ObjectResponse<Message> sendMessage(Snowflake<Channel> channelID, std::string message, bool tts = false);
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
If you want to send a new line, use ``\\\\n``. Normal escape chars do not work, use ``\\\\`` for escapes.
</aside>

### Parameters
<table>
  <tbody>
      <tr><td><strong>channelID</strong></td>
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
bool SleepyDiscord::DiscordClient::addReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji);
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
      <td><strong>channelID</strong></td>
      <td>The id of the channel with the message you want to add a reaction to</td>
    </tr>
    <tr>
      <td><strong>messageID</strong></td>
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
Message editMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string newMessage);
Message editMessage(Message message, std::string newMessage);
```

Edits an existing Message

### Parameters
<table>
  <tbody>
      <tr><td><strong>channelID</strong></td>
        <td>The id of the channel with the message that you want to edit</td></tr>
      <tr><td><strong>messageID</strong></td>
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
bool pinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID);
```

Puts a message into the Pinned Messages of a channel

### Parameters
<table>
  <tbody>
      <tr><td><strong>channelID</strong></td>
        <td>The id of the channel with the message that you want to pin</td></tr>
      <tr><td><strong>messageID</strong></td>
        <td>The id of the message you want to pin</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## unpinMessage

```cpp 
bool unpinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID);
```

Removes a message from the Pinned Messages in a channel

### Parameters
<table>
  <tbody>
      <tr><td><strong>channelID</strong></td>
        <td>The id of the channel with the message that you want to unpin</td></tr>
      <tr><td><strong>messageID</strong></td>
        <td>The id of the message you want to unpin</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## deleteMessage

```cpp 
bool deleteMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID);
```

Deletes a message

### Parameters
<table>
  <tbody>
      <tr><td><strong>channelID</strong></td>
        <td>The id of the channel with the messages that you want to delete</td></tr>
      <tr><td><strong>messageID</strong></td>
        <td>An array of ids of the messages you want to delete</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## editNickname

```cpp 
bool editNickname(Snowflake<Server> serverID, std::string newNickname);
```

Changes the name that is displayed on a server

### Parameters
<table>
  <tbody>
      <tr><td><strong>serverID</strong></td>
        <td>The id of the server where you want to this nickname</td></tr>
      <tr><td><strong>newNickname</strong></td>
        <td>The Nickname that you want your bot to have</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## addRole

```cpp 
bool addRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID);
```

Gives a member a role on a server 

### Parameters
<table>
  <tbody>
      <tr><td><strong>serverID</strong></td>
        <td>The id of the server with the user you want to give the role to</td></tr>
      <tr><td><strong>memberID</strong></td>
        <td>The id of the user that you want to give the role to</td></tr>
      <tr><td><strong>roleID</strong></td>
        <td>The id of role that you want to give</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## removeRole

```cpp 
bool removeRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID);
```

Takes away a role from a member on a server

### Parameters
<table>
  <tbody>
      <tr><td><strong>serverID</strong></td>
        <td>The id of the server with the user you want to remove the role from</td></tr>
      <tr><td><strong>memberID</strong></td>
        <td>The id of the user that you want to remove the role from</td></tr>
      <tr><td><strong>roleID</strong></td>
        <td>The id of role that you want to remove from the user</td></tr>
  </tbody>
</table>

### Return value
Returns ``true`` on success

## kickMember

```cpp 
bool kickMember(Snowflake<Server> serverID, Snowflake<User> userID);
```

Removes a member from a server

### Parameters
<table>
  <tbody>
      <tr><td><strong>serverID</strong></td>
        <td>The id of the server with the user you want to remove</td></tr>
      <tr><td><strong>memberID</strong></td>
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

Lets you disconnect from discord and stops the client

## run

```cpp 
virtual void run();
```

For more information, check out [custom websockets](#custom-websockets)

## request
<aside class="warning">
Please do not use unless you have a good reason.
</aside>

```cpp
Response request(const RequestMethod method, const std::string url, const std::string jsonParameters = "", const std::initializer_list<Part>& multipartParameters = {});
Response request(const RequestMethod method, const std::string url, const std::initializer_list<Part>& multipartParameters);
```

Used to make a request to Discord. For more information, see [Session](#session).

## path
```cpp
const std::string path(const char* source, ...);
```

Usually used with the request function. This creates a path by putting together the url and the parameters.

### Parameters
<table>
  <tbody>
      <tr><td><strong>source</strong></td>
        <td>The link with ``{`` and ``}`` to specify where to place the parameters</td></tr>
      <tr><td><strong>...</strong></td>
        <td>The parameters in std::string. Parameters should go in order from when they appear in source</td></tr>
  </tbody>
</table>

<aside class="note">
Any parameter that's not a std::string will cause issues. If you are using a hard coded string, like ``"this"``, make sure it's a std::string.
</aside>

### Return value
The url with all ``{``, ``}``, and in between replaced with the parameters.

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
			SleepyDiscord::Message message = sendMessage(message.channelID, "Hello " + message.author.username);
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

Objects used to represent things from Discord, these include things like messages, users, server, etc.

## Snowflake
Most Discord Objects have ids, these ids have the type called Snowflake. Snowflakes are 64 bit unsigned ints, currently Sleepy Discord stores them as strings. This is because Discord sends and receives Snowflakes as strings, because unsigned 64 bit integer support is not something every language has. By passing snowflakes as a string, you can guarantee that the receiving language will not try to change it. For example, languages like PHP stores all numbers as doubles or 64 bit floats. In theory, the json standard supports 53 bit signed integers.

### Structure
<pre>
<img src="images/UxWvdYD.png">
</pre>
<table>
  <tbody>
      <tr><td><strong>Timestamp</strong></td>
        <td>42 bits - milliseconds since the first second of 2015</td></tr>
      <tr><td><strong>Internal worker ID</strong></td>
        <td>5 bits - Internally Discord has servers that create snowflakes, this id unique to each generating sever</td></tr>
      <tr><td><strong>Internal process ID</strong></td>
        <td>5 bits - Same thing as the worker ID but unique to each generating process</td></tr>
      <tr><td><strong>Increment</strong></td>
        <td>10 bits - a number that is incremented for every generated ID on the process</td></tr>
  </tbody>
</table>

### But if C++ has support for unsigned 64 bit integer, then why does Sleepy Discord store them as strings?
That's a good question. The fact that they are given to the client as strings and send to Discord as a string, is the main reason why. However, the disadvantages are that the strings take up more memory then 64 bits, and that you will need to convert them to an int if you want to so some math operations with them. However, you cannot send Snowflakes as integers to Discord, as that will give you an error.

# Message

```cpp
struct Message : public DiscordObject {
```

An object that represents a message from Discord. Also very similar to [the message object from the API](https://discordapp.com/developers/docs/resources/channel#message-object)

## (constructor)
```cpp
Message(const std::string * rawJson);
Message(BaseDiscordClient* client, std::string channelID, std::string message, bool tts = false);
```

Initializes the Message object. However the two constructors do different things. As the 2nd one also sends a message and initializes the message object, the first one just initializes the message object. In the 2nd constructor's implementation, it uses the first constructor to initializes the message object.

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
``true`` when the message starts with the given string

## length

```cpp
int length();
```

### Return value
The content length of the message

## isMentioned

```cpp
bool isMentioned(const std::string id);
bool isMentioned(User& _user);
```

### Parameters
<table>
  <tbody>
      <tr><td><strong>id</strong></td>
        <td>The id of the user that you want to check that they were mentioned or not</td></tr>
      <tr><td><strong>_user</strong></td>
        <td>The user object of the user you want to check that they were mentioned or not </td></tr>
  </tbody>
</table>

### Return value
``true`` if the user was mentioned

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

Compares the ids of two messages

###Parameters
<table>
  <tbody>
      <tr><td><strong>message</strong></td>
        <td>The message that is being compared</td></tr>
  </tbody>
</table>

### Return value
``true`` when the ids of the two messages are the same

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
``true`` when the two Users have the same id

# Custom Session

```cpp
class CustomSession : public GenericSession {
```

Sessions are used for http requests, it's essentially a wrapper for any http library for Sleepy Discord. CustomSession is wrapper for a wrapper of http requests and responses. [To learn more about Sessions, click here.](#session)

###Member types
<table>
  <tbody>
      <tr><td><strong>CustomInit</strong></td>
        <td>A function pointer to a function that returns a new Session</td></tr>
  </tbody>
</table>

## (constructor)

```cpp
static CustomInit init;
CustomSession() : session(init()) {}
```

> This how the CustomSession constructor calls your custom Session.

```cpp
class mySession : public SleepyDiscord::GenericSession{
	...  //imagine a filled out Session class
}
SleepyDiscord::CustomInit SleepyDiscord::Session::init = []()->SleepyDiscord::GenericSession* { return new mySession; };	//init the custom session
```

The constructor of the CustomSession class calls init to get a pointer to a new Session.

# Session

```cpp
class GenericSession {
```

```cpp
typedef CPRSession Session;
```

```cpp
typedef CustomSession Session;
```

Session is a class that wraps any http library, for now the library only supports cpr. However this can easily change in the future thanks to the Session class.

Declared in ``http.h``

###Member types
<table>
  <tbody>
      <tr><td><strong>Response</strong></td>
        <td>When you make a request, it sends back a response. This holds the response from the request.</td></tr>
      <tr><td><strong>Part</strong></td>
        <td>Used for multipart requests, is stores the name and file or value</td></tr>
  </tbody>
</table>

## setUrl

```cpp
virtual void setUrl(const std::string& url) = 0;
```

## setBody

```cpp
virtual void setBody(const std::string* jsonParameters) = 0;
```

###Parameters
<table>
  <tbody>
      <tr><td><strong>jsonParameters</strong></td>
        <td>The body of the Session, usually a json</td></tr>
  </tbody>
</table>

## setHeader

```cpp
virtual void setHeader(const std::vector<SleepyDiscord::HeaderPair>& header) = 0;
```

###Parameters
<table>
  <tbody>
      <tr><td><strong>header</strong></td>
        <td>A vector of stuff in the header</td></tr>
  </tbody>
</table>

### HeaderPair
Represents one line of the header
<table>
  <tbody>
      <tr><td><strong>name</strong></td>
        <td>The name of the value, or the left of the header field</td></tr>
      <tr><td><strong>value</strong></td>
        <td>The value, or the right of the header field</td></tr>
  </tbody>
</table>

## setMultipart

```cpp
virtual void setMultipart(const std::initializer_list<Part>& parts) = 0;
```

## Request Methods

Everything else in the Session class makes the request, and returns the response from the request. However each one uses a different request method. [Here's a useful wikipedia about them.](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol#Request_methods)

### Return value
[The response](#response)

## Post

```cpp
virtual Response Post() = 0;
```

## Patch

```cpp
virtual Response Patch() = 0;
```

## Delete

```cpp
virtual Response Delete() = 0;
```

## Get

```cpp
virtual Response Get() = 0;
```

## Put

```cpp
virtual Response Put() = 0;
```

## Response

```cpp
struct Response {
	int32_t statusCode;
	std::string text;
	std::map<std::string, std::string> header;
};
```

When you make a request, a response is returned. The Response struct stores the response from a request.

<aside class="note">
Response is a separate part of Session
</aside>

# Custom Websockets

Without websockets, Discord can't get anything in real time, because http only does things after a request. Think of it as the difference between active and passive. Just like CustomSession, Sleepy Discord's websockets are customizable. Right now, there isn't a special class for websockets, it's part of the ``BaseDiscordClient`` class.

## run

```cpp
virtual void run();
```

A function called by the user to run the websocket client when there's 2 or less threads that can be used for Sleepy Discord

<aside class="warning">
All functions <b>below</b> should be specified as private
</aside>

## connect

```cpp
virtual bool connect(const std::string & uri) { return false; }
```

Called when Sleepy Discord wants to connect.

### Return value
True on a successful connection.

## disconnect
```cpp
virtual void disconnect(unsigned int code, const std::string reason) {}
```

The function that Sleepy Discord uses to disconnect

## send
```cpp
void send(std::string message);
```

A function used for sending things like heartbeats and status updates

## runAsync
```cpp
virtual void runAsync();
```

Runs the websocket client on another thread. This is the function that Sleepy Discord calls when it's told to run on 3 or more threads. Generally, this function should just make a new thread and call run on the new thread.

## SLEEPY LOCK CLIENT FUNCTIONS

```cpp
class WebSocketDiscordClient : public BaseDiscordClient {
private:
	SLEEPY_LOCK_CLIENT_FUNCTIONS
};
```

A macro that is a must for any Discord Clients that will be used by others, that may include you. It specifies functions that a normal user should not touch as private.

# Preprocessor Directives

Sleepy Discord uses some preprocessor directives such as ``#define`` and ``#ifdef``. This is so that Sleepy Discord can be compiled in many different situations. If you are having trouble compiling Sleepy Discord, these might help, but make sure you know what they do because they will disable or add features.

## SLEEPY ONE THREAD

```make
SLEEPY_ONE_THREAD
-DSLEEPY_ONE_THREAD
```
Disables anything that has to do with threads, because threads or ``std::threads`` don't work on everything. Currently there's no way to add in thread support of your own device yet.

## SLEEPY CUSTOM SESSION

```make
SLEEPY_CUSTOM_SESSION
-DSLEEPY_CUSTOM_SESSION
```
Makes Sleepy Discord use the CustomSession Class for sessions, This allows you to use any http library you like to use. [Click here for info on the CustomSession Class](#CustomSession)

## SLEEPY USE HARD CODED GATEWAY

```make
SLEEPY_USE_HARD_CODED_GATEWAY
-DSLEEPY_USE_HARD_CODED_GATEWAY
```
This makes Sleepy Discord skip connecting to ``api/gateway`` to get the gateway and instead it uses the gateway that is hard coded into Sleepy Discord.
