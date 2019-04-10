---
title: Using only Sleepy Discord

language_tabs:

toc_footers:

includes:

search: true
---

# Using only Sleepy Discord

There are at 3 things that the library needs in order to function: WebSocket Secure, HTTPS, and Async Input/Output. The library splits this up into a 2 main parts: the Websocket client and Sessions. There are also a few optional parts that allow for optional features such as voice. Currently, The only one optional part is UDP.

<aside> There are a actually a few libraries that is part of Sleepy Discord. They are json and string view libraries. However, those are cross-platform and shouldn't cause any issues with compatibility. </aside>

## HTTPS

```cpp
class MySession : public SleepyDiscord::GenericSession {
	MySession() = default;
}
```

You'll need to create a class that inherits ``GenericSession``.

This rest is mostly self-explanatory. You basicity implement functions that do what it the function name suggest.

```cpp
void setUrl(const std::string& _url) override;
```

This is self-explanatory.

```cpp
void setBody(const std::string* body) override;
```

body is the data that will be sent in the body of the HTTP request.

```cpp
void setHeader(const std::vector<SleepyDiscord::HeaderPair>& header) override;
```

header is a list of header fields. ``SleepyDiscord::HeaderPair`` has two variables, name and value.

```cpp
void setMultipart(const std::initializer_list<SleepyDiscord::Part>& parts) override;
```

Like header, parts is a list of parts. SleepyDiscord::Part has 3 variables, name, value, and isFile.

```cpp
SleepyDiscord::Response Post  () override;
SleepyDiscord::Response Patch () override;
SleepyDiscord::Response Delete() override;
SleepyDiscord::Response Get   () override;
SleepyDiscord::Response Put   () override;
```

This sends the https request. The only difference between each function is that they use different HTTP methods.

```cpp
//Execute this line at the start of your program
SleepyDiscord::CustomInit SleepyDiscord::Session::init = 
	[]()->SleepyDiscord::GenericSession* { return new MySession; };
```

You need to set SleepyDiscord::Session::init to a function that returns a pointer to a new Session. You can put this in the beginning of your cpp file after including the header file with your session class.

## WebSockets

```cpp
class MyWebsocketConnection : public SleepyDiscord::GenericWebsocketConnection {
public:
	MyWebsocketConnection() {}
	MyWebsocketConnection(std::string token) {
		start(token);
	}

	MyConnectionHandle handle;
}
```

You are going to need a WebSocket Connection class that the library will pass around in a ``std::shared_ptr<GenericWebsocketConnection>`` called ``WebsocketConnection``. ``GenericWebsocketConnection`` is an empty class so this acts like a ``std::shared_ptr<void>``. You can put anything you want in this class, but you'll likely need to handle in there. You'll also likely want to store a reference to the ``GenericMessageReceiver`` in it, but it's optional if you don't want to. We'll talk about ``GenericMessageReceiver`` later. You'll also need to call ``start`` at some point, the constructor isn't a bad place to do this.

```cpp
class MyWebsocketClient : public SleepyDiscord::BaseDiscordClient {
	MyWebsocketClient() = default;
}

//The typedef is optional.
typedef MyWebsocketClient DiscordClient;
```
The WebSocket client is actually a child of The ``BaseDiscordClient`` and using a typedef is renamed to ``DiscordClient``.

```cpp
//Add these function decelerations to your WebsocketClient
//Optional: These should can be set to private
bool connect(
	const std::string & uri,
	GenericMessageReceiver* messageProcessor,
	WebsocketConnection& connection
) override {
	MyConnectionHandle handle = websocket.connect(uri); //connect and get handle from websocket library
	connection = std::make_shared<MyWebsocketConnection>(messageProcessor, handle);
	return false; //you don't need to return a bool, it was related to a removed feature.
}

void disconnect(
	unsigned int code,
	const std::string reason,
	WebsocketConnection& connection
) override {
	connection.get<MyWebsocketConnection>().handle.disconnect()
}

void send(
	std::string message,
	WebsocketConnection& connection
) override {
	connection.get<MyWebsocketConnection>().handle.send(message);
}
```

There are 3 virtual functions that you need to override: connect, disconnect, and send. In ``connect``, set ``connection`` to a new connection object with you ``WebsocketConnection`` class. Use ``std::shared_ptr``'s ``get`` to get your ``WebSocketConnection``.

```cpp
//Add the equivalent code to your connect function
websocket.onOpen = [=]() {
	messageProcessor->initialize();
}

websocket.onClose = [=](int16_t closeCode) {
	messageProcessor->processCloseCode(closeCode);
}

websocket.onMessage = [=](std::string message) {
	messageProcessor->processMessage(handle.getMessage(message))
}

websocket.onFail = [=]() {
	messageProcessor->handleFailToConnect();
}
```

This is where you'll need ``messageProcessor``, you'll need to call it's member functions in the websocket callbacks. If you can't capture ``messageProcessor``, then you'll want to have a reference to it in your connection class.

## Async I/O

```cpp
//Add these function declarations to your WebsocketClient class
void run();
Timer schedule(TimedTask code, const time_t milliseconds) override;
```

There's 1 function you'll need to implement, ``schedule``. ``run`` is optional. Async I/O is done inside the ``WebSocketClient`` so place them in there.

```cpp
//Add the equivalent code
Timer MyWebsocketClient::schedule(TimedTask code, const time_t milliseconds) {
	Library::Timer timer = Library::createTimer(code, milliseconds);
	return Timer([timer](){
		timer.stop();
	});
}
```

Create a timer that will call ``code`` in ``milliseconds`` milliseconds. Then, return a function to stop the timer.

```cpp
//Here's an example of run
void run() {
	Library::run();
}
```