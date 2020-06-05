#pragma once
#include <string>
#include "discord_object_interface.h"
#include "user.h"
#include "channel.h"
#include "snowflake.h"
#include "cache.h"

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
		ServerMember() = default;
		//ServerMember(const std::string * rawJson);
		ServerMember(const nonstd::string_view & rawJSON);
		ServerMember(const json::Value& json);
		//ServerMember(const json::Values values);
		User user;
		std::string nick;
		std::vector<Snowflake<Role>> roles;
		std::string joinedAt;
		bool deaf = false;
		bool mute = false;

		inline operator User&() {
			return user;
		}

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair                           (&ServerMember::user    , "user"     , json::OPTIONAL_FIELD),
				json::pair                           (&ServerMember::nick    , "nick"     , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&ServerMember::roles   , "roles"    , json::OPTIONAL_FIELD),
				json::pair                           (&ServerMember::joinedAt, "joined_at", json::OPTIONAL_FIELD),
				json::pair                           (&ServerMember::deaf    , "deaf"     , json::OPTIONAL_FIELD),
				json::pair                           (&ServerMember::mute    , "mute"     , json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	struct Server : public IdentifiableDiscordObject<Server> {
		//~Server();
		Server() = default;
		//Server(const std::string * rawJson);
		Server(const nonstd::string_view & rawJSON);
		Server(const json::Value& json);
		//Server(const json::Values values);
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
		int verificationLevel;
		int defaultMessageNotifications;
		std::list<Role> roles;
		//voice_states
		//emojis
		//features
		bool unavailable;

		//presences
		int MFALevel;
		std::string joinedAt;
		
		//those are only filled in from the onServer event
		bool large;

		std::list<ServerMember> members;
		std::list<Channel> channels;

		std::list<ServerMember>::iterator findMember(const Snowflake<User> userID);
		std::list<Channel>::iterator findChannel(const Snowflake<Channel> channelID);
		std::list<Role>::iterator findRole(const Snowflake<Role> roleID);

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair                           (&Server::ID                         , "id"                           , json::REQUIRIED_FIELD),
				json::pair                           (&Server::name                       , "name"                         , json::REQUIRIED_FIELD),
				json::pair                           (&Server::icon                       , "icon"                         , json::NULLABLE_FIELD ),
				json::pair                           (&Server::splash                     , "splash"                       , json::NULLABLE_FIELD ),
				json::pair                           (&Server::ownerID                    , "owner_id"                     , json::OPTIONAL_FIELD ),
				json::pair<json::EnumTypeHelper     >(&Server::permissions                , "permissions"                  , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::region                     , "region"                       , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::AFKchannelID               , "afk_channel_id"               , json::NULLABLE_FIELD ),
				json::pair                           (&Server::AFKTimeout                 , "afk_timeout"                  , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::embedEnable                , "embed_enabled"                , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::embedChannelID             , "embed_channel_id"             , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::verificationLevel          , "verification_level"           , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::defaultMessageNotifications, "default_message_notifications", json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&Server::roles                      , "roles"                        , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::unavailable                , "unavailable"                  , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::MFALevel                   , "mfa_level"                    , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::joinedAt                   , "joined_at"                    , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::large                      , "large"                        , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&Server::members                    , "members"                      , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&Server::channels                   , "channels"                     , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

	struct UnavailableServer : public IdentifiableDiscordObject<Server> {
		UnavailableServer() = default;
		//UnavailableServer(const std::string * rawJson);
		UnavailableServer(const nonstd::string_view & rawJSON);
		UnavailableServer(const json::Value& json);
		//UnavailableServer(const json::Values values);

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&UnavailableServer::ID, "id", json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};

	class ServerCache : public Cache<Server> {
	public:
		using Cache<Server>::Cache;
		ServerCache() : Cache() {} //for some odd reason the default constructor isn't inherited
		ServerCache(Cache<Server> list) : Cache<Server>(list) {}

		/*
		//Linear time complexity if unordered map: to do figure out how to do this with constant time complexity
		template<class Container, class Object>
		iterator findOneWithObject(Container Server::*list, const Snowflake<Object>& objectID) {
			return std::find_if(begin(), end(), [&objectID, list](Server& server) {
				auto result = objectID.findObject(server.*list);
				return result != std::end(server.*list);
			});
		}
		*/

		inline const_iterator findSeverWith(const Snowflake<Channel>& channelID) {
			return findOneWithObject(&Server::channels, channelID);
		}

		inline const_iterator findServerWith(const Snowflake<Role> roleID) {
			return findOneWithObject(&Server::roles, roleID);
		}

		//Linear time complexity if using list
		//Usually Constant time complexity if using unordered maps
		inline iterator findServer(const Snowflake<Server> serverID) {
			return serverID.findObject(*this);
		}
	};

	struct ServerEmbed : public DiscordObject {
		ServerEmbed() = default;
		//ServerEmbed(const std::string * rawJson);
		ServerEmbed(const nonstd::string_view & rawJSON);
		ServerEmbed(const json::Value& json);
		//ServerEmbed(const json::Values values);
		bool enabled;
		Snowflake<ServerEmbed> channelID;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&ServerEmbed::enabled  , "enabled"   , json::REQUIRIED_FIELD),
				json::pair(&ServerEmbed::channelID, "channel_id", json::NULLABLE_FIELD )
			);
		JSONStructEnd
	};
}