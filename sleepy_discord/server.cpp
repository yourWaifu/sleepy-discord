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

	Server::Server(const json::Value & json) :
		Server(json::fromJSON<Server>(json)) {
	}

	UnavailableServer::UnavailableServer(const json::Value & json) :
		UnavailableServer(json::fromJSON<UnavailableServer>(json)) {
	}

	ServerWidget::ServerWidget(const json::Value & json) :
		ServerWidget(json::fromJSON<ServerWidget>(json)) {
	}

	ServerMembersRequest::ServerMembersRequest(const json::Value& json) :
		ServerMembersRequest(json::fromJSON<ServerMembersRequest>(json)) {
	}
}