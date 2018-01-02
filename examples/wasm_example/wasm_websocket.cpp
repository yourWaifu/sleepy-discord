#include "wasm_websocket.h"
#include "wasm_session.h"

std::vector<DiscordClient*> clientArray;

WebAssemblyDiscordClient::WebAssemblyDiscordClient(const std::string token) {
	std::cout << "cWebAssemblyDiscordClient" << '\n';
	clientArray.push_back(this);
	start(token, 1);
}

WebAssemblyDiscordClient::~WebAssemblyDiscordClient() {
	std::cout << "c~WebAssemblyDiscordClient" << '\n';
	for (auto c = begin (clientArray); c != end (clientArray); ++c) {
		if (c[0]->getHandle() == handle) {
			clientArray.erase(c);
			return;
		}
	}
}

DiscordClient* getClient(const int& handle) {
	DiscordClient* client = nullptr;
	for (DiscordClient* c : clientArray)  //find client with the same handle
		if (c->getHandle() == handle) {
			client = c;
			break;
		}
	return client;
}

extern "C" EMSCRIPTEN_KEEPALIVE void passMessageToClient(int handle, char* message) {
	DiscordClient* client = getClient(handle);
	if (client == nullptr) {
		free(message);
		return;
	}
	client->passMessageToClient(message);
	//Memory is allocated when converting JS strings to char arrays
	free(message);
}

extern "C" EMSCRIPTEN_KEEPALIVE void heartbeatClient(int handle) {
	DiscordClient* client = getClient(handle);
	if (client == nullptr) {
		return;
	}
	client->sentHeartbeat();
}

#include "sleepy_discord/standard_config.h"