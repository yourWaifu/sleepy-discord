#pragma once
#include <string>
#include "discord_object_interface.h"
#include "user.h"

namespace SleepyDiscord {
	class BaseDiscordClient;

	struct Server : public DiscordObject {
		~Server();
		Server(const std::string * rawJson);
		std::string id;
		std::string name;
		std::string icon;
		std::string splash;
		std::string owner_id;
		std::string region;
		std::string afk_channel_id;
		double afk_timeout;
		bool embed_enable;
		std::string embed_channel_id;
		double verfication_level;
		//voice_states
		//roles
		//emojis
		//features
		bool unavailable;

		//presences
		double mfa_level;
		//members
		bool large;
		std::string joined_at;
		double default_message_notifications;
	};

	struct UnavailableServer : public DiscordObject {
		UnavailableServer(const std::string * rawJson);
		std::string id;
		bool unavailable;
	};

	struct ServerEmbed : public DiscordObject {
		ServerEmbed(const std::string * rawJson);
		bool enabled;
		std::string channel_id;
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
		ServerMember(BaseDiscordClient* client, std::string server_id, std::string user_id);
		ServerMember(const std::string * rawJson);
		User user;
		std::string nick; //nullable
		std::vector<std::string> roles;
		std::string joined_at;
		bool deaf;
		bool mute;
	};
}