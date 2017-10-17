#pragma once
#include <string>
#include "discord_object_interface.h"
#include "user.h"
#include "channel.h"

namespace SleepyDiscord {
	class BaseDiscordClient;

	struct Server : public DiscordObject {
		~Server();
		Server();
		Server(const std::string * rawJson);
		std::string id;
		std::string name;
		std::string icon;
		std::string splash;
		std::string owner_id;
		std::string region;
		std::string afk_channel_id;
		int afk_timeout;
		bool embed_enable;
		std::string embed_channel_id;
		int verfication_level;
		//voice_states
		//roles
		//emojis
		//features
		bool unavailable;

		//presences
		int mfa_level;
		//members
		std::string joined_at;
		int default_message_notifications;

		//those are only filled in from the onServer event
		std::vector<Channel> channels;
		bool large;
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