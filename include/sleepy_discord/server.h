#pragma once
#include <string>
#include "discord_object_interface.h"
#include "user.h"
#include "channel.h"
#include "stage_instance.h"
#include "snowflake.h"
#include "cache.h"
#include "voice.h"

namespace SleepyDiscord {
	enum Permission : uint64_t;
	struct Role;
	struct StageInstance;
	
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
		ServerMember(const nonstd::string_view& json) :
			ServerMember(json::fromJSON<ServerMember>(json)) {}
		ServerMember(const json::Value& json);
		//ServerMember(const json::Values values);
		User user;
		std::string nick;
		std::vector<Snowflake<Role>> roles;
		std::string joinedAt;
		bool deaf = false;
		bool mute = false;
		Permission permissions;
		bool pending = false;

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
				json::pair                           (&ServerMember::mute    , "mute"     , json::OPTIONAL_FIELD),
				json::pair<UInt64StrTypeHelper>      (&ServerMember::permissions, "permissions", json::OPTIONAL_FIELD),
				json::pair                           (&ServerMember::pending , "pending"  , json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};
    
	struct Server : public IdentifiableDiscordObject<Server> {
		~Server() = default;
		Server() = default;
		Server(const nonstd::string_view& json) :
			Server(json::fromJSON<Server>(json)) {}
		Server(const json::Value& json);
		std::string name;
		std::string icon;
		std::string splash;
		std::string discoverySplash;
		Snowflake<User> ownerID;
		Permission permissions;
		std::string region;
		std::string AFKchannelID;
		int AFKTimeout;
		bool embedEnable;
		std::string embedChannelID;
		int verificationLevel;
		int defaultMessageNotifications;
		int explicitContentFilter;
        
		std::list<Role> roles;
		std::list<VoiceState> voiceStates;
		//emojis
		std::vector<std::string> features;
		bool unavailable;
		std::vector<StageInstance> stageInstances;
		Snowflake<DiscordObject> applicationID;
		Snowflake<Channel> systemChannelID;
		int systemChannelFlags;
		Snowflake<Channel> rulesChannelID;
		//presences
		int MFALevel;
		std::string joinedAt;
		int maxMembers = 0;
		std::string vanityUrlCode;
		std::string description;
		std::string banner;
		int premiumTier = 0;
		int premiumSubscriptionCount = 0;
		std::string preferredLocale;
		Snowflake<Channel> publicUpdatesChannelID;
		int maxVideoChannelUsers;
		int approximateMemberCount;
		int approximatePresenceCount;
		int nsfwLevel = 0;
        
		//those are only filled in from the onServer event
		bool large;
		int memberCount = 0;
		std::list<ServerMember> members;
		std::list<Channel> channels;

		std::list<ServerMember>::iterator findMember(const Snowflake<User> userID);
		std::list<Channel>::iterator findChannel(const Snowflake<Channel> channelID);
		std::list<Role>::iterator findRole(const Snowflake<Role> roleID);

		JSONStructStart
			std::make_tuple(
				json::pair                           (&Server::ID                         , "id"                           , json::REQUIRIED_FIELD),
				json::pair                           (&Server::name                       , "name"                         , json::REQUIRIED_FIELD),
				json::pair                           (&Server::icon                       , "icon"                         , json::NULLABLE_FIELD ),
				json::pair                           (&Server::splash                     , "splash"                       , json::NULLABLE_FIELD ),
				json::pair                           (&Server::discoverySplash            , "discovery_splash"             , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::ownerID                    , "owner_id"                     , json::OPTIONAL_FIELD ),
				json::pair<UInt64StrTypeHelper      >(&Server::permissions                , "permissions"                  , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::region                     , "region"                       , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::AFKchannelID               , "afk_channel_id"               , json::NULLABLE_FIELD ),
				json::pair                           (&Server::AFKTimeout                 , "afk_timeout"                  , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::embedEnable                , "widget_enabled"               , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::embedChannelID             , "widget_channel_id"            , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::verificationLevel          , "verification_level"           , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::defaultMessageNotifications, "default_message_notifications", json::OPTIONAL_FIELD ),
				json::pair                           (&Server::explicitContentFilter      , "explicit_content_filter"      , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&Server::roles                      , "roles"                        , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&Server::features                   , "features"                     , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::unavailable                , "unavailable"                  , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::MFALevel                   , "mfa_level"                    , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::applicationID              , "application_id"               , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::systemChannelID            , "system_channel_id"            , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::systemChannelFlags         , "system_channel_flags"         , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::rulesChannelID             , "rules_channel_id"             , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::joinedAt                   , "joined_at"                    , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::large                      , "large"                        , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::memberCount                , "member_count"                 , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&Server::voiceStates                , "voice_states"                 , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&Server::members                    , "members"                      , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&Server::channels                   , "channels"                     , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::maxMembers                 , "max_members"                  , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::vanityUrlCode              , "vanity_url_code"              , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::description                , "description"                  , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::banner                     , "banner"                       , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::premiumTier                , "premium_tier"                 , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::premiumSubscriptionCount   , "premium_subscription_count"   , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::preferredLocale            , "preferred_locale"             , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::publicUpdatesChannelID     , "public_updates_channel_id"    , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::maxVideoChannelUsers       , "max_video_channel_users"      , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::approximateMemberCount     , "approximate_member_count"     , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::approximatePresenceCount   , "approximate_presence_count"   , json::OPTIONAL_FIELD ),
				json::pair                           (&Server::nsfwLevel                  , "nsfw_level"                   , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&Server::stageInstances             , "stage_instances"              , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

	struct UnavailableServer : public IdentifiableDiscordObject<Server> {
		UnavailableServer() = default;
		//UnavailableServer(const std::string * rawJson);
		UnavailableServer(const nonstd::string_view& json) :
			UnavailableServer(json::fromJSON<UnavailableServer>(json)) {}
		UnavailableServer(const json::Value& json);
		//UnavailableServer(const json::Values values);

		enum class AvailableFlag {
			NotSet = -2,
			Unavaiable = true,
			avaiable = false,
		};
		AvailableFlag unavailable = AvailableFlag::NotSet;

		JSONStructStart
			std::make_tuple(
				json::pair(&UnavailableServer::ID, "id", json::REQUIRIED_FIELD),
				json::pair<json::EnumTypeHelper>
				(&UnavailableServer::unavailable, "unavailable", json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	template<>
	struct GetDefault<UnavailableServer::AvailableFlag> {
		static inline const UnavailableServer::AvailableFlag get() {
			return UnavailableServer::AvailableFlag::NotSet;
		} 
	};

	template<>
	struct GetEnumBaseType<UnavailableServer::AvailableFlag> {
		//this makes the json wrapper know to use getBool instead of getInt
		using Value = bool; 
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

	struct ServerWidget : public DiscordObject {
		ServerWidget() = default;
		//ServerWidget(const std::string * rawJson);
		ServerWidget(const nonstd::string_view& json) :
			ServerWidget(json::fromJSON<ServerWidget>(json)) {}
		ServerWidget(const json::Value& json);
		//ServerWidget(const json::Values values);
		bool enabled;
		Snowflake<Channel> channelID;

		//const static std::initializer_list<const char*const> fields;
		JSONStructStart
			std::make_tuple(
				json::pair(&ServerWidget::enabled  , "enabled"   , json::REQUIRIED_FIELD),
				json::pair(&ServerWidget::channelID, "channel_id", json::NULLABLE_FIELD )
			);
		JSONStructEnd
	};

	struct ServerMembersRequest {
		ServerMembersRequest() = default;
		ServerMembersRequest(const json::Value& json);
		ServerMembersRequest(const nonstd::string_view& json) :
			ServerMembersRequest(json::fromJSON<ServerMembersRequest>(json)) {}
		Snowflake<Server> serverID;
		//since empty and undefined mean different things to the API, we need optional
		tl::optional<std::string> query;
		int limit = 0;
		bool presence = false;
		std::vector<Snowflake<User>> userIDs;
		std::string nonce;
		
		JSONStructStart
			std::make_tuple(
				json::pair                           (&ServerMembersRequest::serverID, "guild_id" , json::REQUIRIED_FIELD),
				json::pair<json::OptionalTypeHelper >(&ServerMembersRequest::query   , "query"    , json::OPTIONAL_FIELD ),
				json::pair                           (&ServerMembersRequest::limit   , "limit"    , json::REQUIRIED_FIELD),
				json::pair                           (&ServerMembersRequest::presence, "presences", json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&ServerMembersRequest::userIDs , "user_ids" , json::OPTIONAL_FIELD ), 
				json::pair                           (&ServerMembersRequest::nonce   , "nonce"    , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};
}
