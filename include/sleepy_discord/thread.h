#pragma once
#include "discord_object_interface.h"
#include "server.h"
#include "channel.h"
#include "user.h"

namespace SleepyDiscord {
	struct ThreadMember : public IdentifiableDiscordObject<Channel> {
		ThreadMember() = default;
		ThreadMember(const nonstd::string_view& json) :
			ThreadMember(json::fromJSON<ThreadMember>(json)) {}
		ThreadMember(const json::Value& json);

		Snowflake<User> userID;
		std::string joinTimestamp;
		int flags = 0;
		ServerMember member;

		JSONStructStart
			std::make_tuple(
				json::pair(&ThreadMember::ID, "id", json::OPTIONAL_FIELD),
				json::pair(&ThreadMember::userID, "user_id", json::OPTIONAL_FIELD),
				json::pair(&ThreadMember::joinTimestamp, "join_timestamp", json::OPTIONAL_FIELD),
				json::pair(&ThreadMember::member, "member", json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	struct ThreadListSync : public DiscordObject {
		ThreadListSync() = default;
		ThreadListSync(const nonstd::string_view& json) :
			ThreadListSync(json::fromJSON<ThreadListSync>(json)) {}
		ThreadListSync(const json::Value& json);

		Snowflake<Server> serverID;
		std::vector<Snowflake<Channel>> channelIDs;
		std::vector<Channel> threads;
		std::vector<ThreadMember> members;

		JSONStructStart
			std::make_tuple(
				json::pair                           (&ThreadListSync::serverID  , "guild_id", json::REQUIRIED_FIELD),
				json::pair<json::ContainerTypeHelper>(&ThreadListSync::channelIDs, "channel_ids", json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&ThreadListSync::threads   , "channel_ids", json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&ThreadListSync::members   , "members", json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	struct ThreadMembersUpdate : public IdentifiableDiscordObject<Channel> {
		ThreadMembersUpdate() = default;
		ThreadMembersUpdate(const nonstd::string_view& json) :
			ThreadMembersUpdate(json::fromJSON<ThreadMembersUpdate>(json)) {}
		ThreadMembersUpdate(const json::Value& json);

		Snowflake<Server> serverID;
		int memberCount = 0;
		std::vector<ThreadMember> addedMembers;
		std::vector<Snowflake<User>> removedMemberIDs;

		JSONStructStart
			std::make_tuple(
				json::pair(&ThreadMembersUpdate::ID, "id", json::REQUIRIED_FIELD),
				json::pair(&ThreadMembersUpdate::serverID, "guild_id", json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&ThreadMembersUpdate::addedMembers, "added_members", json::OPTIONAL_FIELD),
				json::pair(&ThreadMembersUpdate::memberCount, "memberCount", json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&ThreadMembersUpdate::removedMemberIDs, "removed_member_ids", json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};
}