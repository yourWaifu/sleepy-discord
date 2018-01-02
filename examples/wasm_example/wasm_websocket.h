#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include "sleepy_discord/sleepy_discord.h"
#include <emscripten.h>
#include <emscripten/bind.h>

#define SLEEPY_DO_NOT_INCLUDE_STANDARD_SLEEP  //without this, the broswer will crash

extern "C" {
	extern void SendWebSocket(int handle, const char* message);
	extern void closeWebSocket(int handle, unsigned int code, const char* reason);
	extern int connectWebSocket(const char* uri);
	extern void runWebSocket(int handle);
	extern void heartbeat(int handle, int heartbeatInterval);
}

class WebAssemblyDiscordClient : public SleepyDiscord::BaseDiscordClient {
public:
	WebAssemblyDiscordClient() {}
	WebAssemblyDiscordClient(const std::string token);
	~WebAssemblyDiscordClient();
	void passMessageToClient(std::string message) {
		processMessage(message);  //note: threads are made in JS
	}
	void sentHeartbeat() {
		std::cout << "csentHeartbeat" << '\n';
		/*if (isReady())*/ heartbeat();
	}
	const int getHandle() {
		return handle;
	}
	void run() {
		std::cout << "crun" << '\n';
		runWebSocket(handle);
		std::cout << "crun end" << '\n';
	}
protected:
#include "sleepy_discord/standard_config_header.h"
private:
	int handle;
	bool connect(const std::string & uri) {
		std::cout << "cconnect" << '\n';
		handle = connectWebSocket(uri.data());
		std::cout << "cconnect end" << handle << '\n';
		return false;	//to do make connect return false on error
	}
	void disconnect(unsigned int code, const std::string reason) {
		std::cout << "cdisconnect" << '\n';
		closeWebSocket(handle, code, reason.data());
	}
	void send(std::string message) {
		std::cout << "csend" << '\n';
		SendWebSocket(handle, message.data());
	}
	void runAsync() {  //note: this function should never be called
		               //as it's already async without calling it.
		runWebSocket(handle);
	}
	void sleep(const unsigned int milliseconds) {

	}
	void onHelloHeartbeatInterval(int heartbeatInterval) {
		::heartbeat(handle, heartbeatInterval);
	}
	SLEEPY_LOCK_CLIENT_FUNCTIONS
};

typedef WebAssemblyDiscordClient DiscordClient;

extern "C" EMSCRIPTEN_KEEPALIVE void passMessageToClient(int handle, char* message);
extern "C" EMSCRIPTEN_KEEPALIVE void heartbeatClient(int handle);


EMSCRIPTEN_BINDINGS(client) {
	emscripten::class_<WebAssemblyDiscordClient>("WebAssemblyDiscordClient")
		.constructor<const std::string>()
		.function("run", &WebAssemblyDiscordClient::run)
		;
}
