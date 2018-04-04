#pragma once
#include <string>
#include "discord_object_interface.h"
#include "user.h"
#include "channel.h"
#include "snowflake.h"

namespace SleepyDiscord {
	class BaseDiscordClient;

	struct Server : public DiscordObject {
		~Server();
		Server();
		Server(const std::string * rawJson);
		Server(const std::vector<std::string> values);
		Snowflake<Server> ID;
		std::string name;
		std::string icon;
		std::string splash;
		Snowflake<User> ownerID;
		std::string region;
		std::string AFKchannelID;
		int afk_timeout;
		bool embedEnable;
		std::string embedChannelID;
		int verficationLevel;
		int defaultMessageNotifications;
		//voice_states
		//roles
		//emojis
		//features
		bool unavailable;

		//presences
		int MFALevel;
		//members
		std::string joinedAt;

		//those are only filled in from the onServer event
		bool large;
		std::vector<Channel> channels;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	struct UnavailableServer : public DiscordObject {
		UnavailableServer(const std::string * rawJson);
		UnavailableServer(const std::vector<std::string> values);
		Snowflake<UnavailableServer> ID;
		bool unavailable;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	struct ServerEmbed : public DiscordObject {
		ServerEmbed(const std::string * rawJson);
		ServerEmbed(const std::vector<std::string> values);
		bool enabled;
		Snowflake<ServerEmbed> channelID;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	/*Guild Member Structure
	Field     Type     Description
	user      object   user object
	nick      string?  this users guild nickname (if one is set)
	roles     array    array of role object id's
	joined_at datetime date the user joined the guild
	deaf      bool     if the user is deafened
	mute      bool     if the user is muted
	*/
	struct ServerMember : public DiscordObject {
		ServerMember() {}
		ServerMember(const std::string * rawJson);
		ServerMember(const std::vector<std::string> values);
		User user;
		std::string nick; //nullable
		std::vector<std::string> roles;
		std::string joinedAt;
		bool deaf;
		bool mute;
	private:
		const static std::initializer_list<const char*const> fields;
	};
}