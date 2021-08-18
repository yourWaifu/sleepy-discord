#include <list>
#include "wasm_websocket.h"
#include "wasm_session.h"

/*This needs to have global scope, so that JS can make calls to them*/
std::list<DiscordClient*> clientList;
std::list<Assignment> assignmentList;

WebAssemblyDiscordClient::WebAssemblyDiscordClient(const std::string token) {
	std::cout << "cWebAssemblyDiscordClient" << '\n';
	clientList.push_front(this);
	start(token, 1);
}

WebAssemblyDiscordClient::~WebAssemblyDiscordClient() {
	std::cout << "c~WebAssemblyDiscordClient" << '\n';
	WebSocketHandle& handle = connection.get<WebSocketHandle>();
}

SleepyDiscord::Timer WebAssemblyDiscordClient::schedule(
	SleepyDiscord::TimedTask code, const time_t milliseconds
) {
	assignmentList.emplace_front(code);
	Assignment& assignment = assignmentList.front();
	assignment.jobID = setTimer(&assignment, milliseconds);
	return SleepyDiscord::Timer([assignment]() {
		stopTimer(assignment.jobID);
		assignmentList.remove(assignment);
	});
}

//insures that we call functions of the correct client
DiscordClient* getClient(const int& handle) {
	for (DiscordClient* c : clientList)  //find client with the same handle
		if (c->connection.get<WebSocketHandle>() == handle)
			return c;
	return nullptr;
}

//see WebAssemblyDiscordClient::passMessageToClient
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

//see WebAssemblyDiscordClient::doAssignment
extern "C" EMSCRIPTEN_KEEPALIVE void doAssignment(Assignment* assignment) {
	assignment->task();
	assignmentList.remove(*assignment);
}

#include "sleepy_discord/standard_config.h"