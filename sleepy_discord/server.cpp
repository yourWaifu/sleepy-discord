#include "server.h"
#include "client.h"
#include "permissions.h"

namespace SleepyDiscord {
	std::list<ServerMember>::iterator Server::findMember(Snowflake<User> userID) {
		return userID.findObject(members.begin(), members.end());
	}

	std::list<Channel>::iterator Server::findChannel(Snowflake<Channel> channelID) {
		return channelID.findObject(channels.begin(), channels.end());
	}

	std::list<Role>::iterator Server::findRole(Snowflake<Role> roleID) {
		return roleID.findObject(roles.begin(), roles.end());
	}

	ServerMember::ServerMember(const json::Value & json) :
		ServerMember(json::fromJSON<ServerMember>(json)) {
		ID = user.ID;
	}
	ServerMember::ServerMember(const nonstd::string_view & json) :
		ServerMember(json::fromJSON<ServerMember>(json)) {
	}

	Server::Server(const json::Value & json) :
		Server(json::fromJSON<Server>(json)) {
	}
	Server::Server(const nonstd::string_view & json) :
		Server(json::fromJSON<Server>(json)) {
	}

	UnavailableServer::UnavailableServer(const json::Value & json) :
		UnavailableServer(json::fromJSON<UnavailableServer>(json)) {
	}
	UnavailableServer::UnavailableServer(const nonstd::string_view & json) :
		UnavailableServer(json::fromJSON<UnavailableServer>(json)) {
	}

	ServerEmbed::ServerEmbed(const json::Value & json) :
		ServerEmbed(json::fromJSON<ServerEmbed>(json)) {
	}
	ServerEmbed::ServerEmbed(const nonstd::string_view & json) :
		ServerEmbed(json::fromJSON<ServerEmbed>(json)) {
	}
}