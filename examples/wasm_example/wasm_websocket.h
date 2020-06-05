#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#define SLEEPY_CUSTOM_CLIENT
#include "sleepy_discord/sleepy_discord.h"
#include <emscripten.h>
#include <emscripten/bind.h>

#define SLEEPY_DO_NOT_INCLUDE_STANDARD_SLEEP  //without this, the broswer will crash

struct Assignment {
public:
	Assignment(SleepyDiscord::TimedTask code) : task(code) {}

	inline bool operator==(const Assignment& right) const {
		return jobID == right.jobID;
	}

	int jobID;
	SleepyDiscord::TimedTask task;
};

extern "C" {
	extern void sendWebSocket(int handle, const char* message);
	extern void closeWebSocket(int handle, unsigned int code, const char* reason);
	extern int connectWebSocket(const char* uri);
	extern void runWebSocket(int handle);
	extern int setTimer(Assignment* assignment, int heartbeatInterval);
	extern void stopTimer(int jobID);
}

struct WebSocketHandle : public SleepyDiscord::GenericWebsocketConnection {
public:
	WebSocketHandle(int _handle, SleepyDiscord::GenericMessageReceiver*& processor) : 
		handle(_handle), messageProcessor(processor) {}

	inline operator int() const {
		return handle;
	}

	inline bool operator==(const WebSocketHandle& right) const {
		return handle == right.handle;
	}

	inline bool operator==(const int& right) const {
		return handle == right;
	}

private:
	int handle;
	SleepyDiscord::GenericMessageReceiver*& messageProcessor;
};

class WebAssemblyDiscordClient : public SleepyDiscord::BaseDiscordClient {
public:
	WebAssemblyDiscordClient() {}
	WebAssemblyDiscordClient(const std::string token);
	~WebAssemblyDiscordClient();

	/*Since the websocket connection is done in JS, JS needs a way to pass
	  the message from JS to the client class when Discord sents data to the
	  client                                                                */
	void passMessageToClient(std::string message) {
		processMessage(message);  //note: threads are made in JS
	}
	
	void run() override {
		std::cout << "crun" << '\n';
		std::cout << "crun end" << '\n';
	}
protected:
	//gives us the standard way of outputing the error
#include "sleepy_discord/standard_config_header.h"
private:

	bool connect( //see docs for info
		const std::string & uri,
		SleepyDiscord::GenericMessageReceiver* messageProcessor,
		SleepyDiscord::WebsocketConnection& connection
	) override {
		std::cout << "cconnect" << '\n';
		connection.forward<WebSocketHandle>(
			connectWebSocket(uri.data()),
			messageProcessor
		);
		std::cout << "cconnect end" << static_cast<int>(connection.get<WebSocketHandle>()) << '\n';
		return false;	//to do make connect return false on error
	}

	void disconnect( //see docs for info
		unsigned int code,
		const std::string reason,
		SleepyDiscord::WebsocketConnection& connection
	) override {
		std::cout << "cdisconnect" << '\n';
		closeWebSocket(connection.get<WebSocketHandle>(), code, reason.data());
	}

	void send(
		std::string message,
		SleepyDiscord::WebsocketConnection& connection
	) override {
		std::cout << "csend" << '\n';
		sendWebSocket(connection.get<WebSocketHandle>(), message.data());
	}
	
	SleepyDiscord::Timer schedule(
		SleepyDiscord::TimedTask code, const time_t milliseconds
	) override;

	SLEEPY_LOCK_CLIENT_FUNCTIONS
};

typedef WebAssemblyDiscordClient DiscordClient;

extern "C" EMSCRIPTEN_KEEPALIVE void passMessageToClient(int handle, char* message);
extern "C" EMSCRIPTEN_KEEPALIVE void doAssignment(Assignment* assignment);


EMSCRIPTEN_BINDINGS(client) {
	emscripten::class_<WebAssemblyDiscordClient>("WebAssemblyDiscordClient")
		.constructor<const std::string>()
		.function("run", &WebAssemblyDiscordClient::run)
		;
}
