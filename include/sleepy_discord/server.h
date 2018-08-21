#pragma once
#include <string>
#include <list>
#include "discord_object_interface.h"
#include "user.h"
#include "channel.h"
#include "snowflake.h"

namespace SleepyDiscord {
	enum Permission : int64_t;
	struct Role;
	
	/*Guild Member Structure
	Field     Type     Description
	user      object   user object
	nick      string?  this users guild nickname (if one is set)
	roles     array    array of role object id's
	joined_at datetime date the user joined the guild
	deaf      bool     if the user is deafened
	mute      bool     if the user is muted
	*/
	struct ServerMember : public IdentifiableDiscordObject<User> {
		ServerMember();
		ServerMember(const std::string * rawJson);
		ServerMember(const std::vector<std::string> values);
		User user;
		std::string nick; //nullable
		std::vector<Snowflake<Role>> roles;
		std::string joinedAt;
		bool deaf;
		bool mute;

		operator User&() {
			return user;
		}
	private:
		const static std::initializer_list<const char*const> fields;
	};

	struct Server : public IdentifiableDiscordObject<Server> {
		~Server();
		Server();
		Server(const std::string * rawJson);
		Server(const std::vector<std::string> values);
		std::string name;
		std::string icon;
		std::string splash;
		Snowflake<User> ownerID;
		Permission permissions;
		std::string region;
		std::string AFKchannelID;
		int AFKTimeout;
		bool embedEnable;
		std::string embedChannelID;
		int verficationLevel;
		int defaultMessageNotifications;
		std::vector<Role> roles;
		//voice_states
		//emojis
		//features
		bool unavailable;

		//presences
		int MFALevel;
		std::string joinedAt;
		
		//those are only filled in from the onServer event
		bool large;

		std::vector<ServerMember> members;
		std::vector<Channel> channels;

		std::vector<ServerMember>::iterator findMember(Snowflake<User> userID);
		std::vector<Channel>::iterator findChannel(Snowflake<Channel> channelID);
	private:
		const static std::initializer_list<const char*const> fields;
	};

	struct UnavailableServer : public IdentifiableDiscordObject<Server> {
		UnavailableServer(const std::string * rawJson);
		UnavailableServer(const std::vector<std::string> values);
		bool unavailable;
	private:
		const static std::initializer_list<const char*const> fields;
	};

	struct ServerCache : public std::list<Server> {
		using std::list<Server>::list;
		ServerCache() : list() {} //for some odd reason the default constructor isn't inherited
		ServerCache(std::list<Server> list) : std::list<Server>(list) {}

		template<class Container, class Object>
		iterator findOnetWithObject(Container Server::*list, const Snowflake<Object>& objectID) {
			return std::find_if(begin(), end(), [&objectID, list](Server& server) {
				auto result = objectID.findObject(server.*list);
				return result != std::end(server.*list);
			});
		}

		inline iterator findSeverWith(const Snowflake<Channel>& channelID) {
			return findOnetWithObject(&Server::channels, channelID);
		}

		inline iterator findServerWith(const Snowflake<Role> roleID) {
			return findOnetWithObject(&Server::roles, roleID);
		}

		inline iterator findServer(const Snowflake<Server> serverID) {
			return std::find_if(begin(), end(), [&serverID](Server& server) {
				return server.ID == serverID;
			});
		}
	};

	struct ServerEmbed : public DiscordObject {
		ServerEmbed(const std::string * rawJson);
		ServerEmbed(const std::vector<std::string> values);
		bool enabled;
		Snowflake<ServerEmbed> channelID;
	private:
		const static std::initializer_list<const char*const> fields;
	};
}