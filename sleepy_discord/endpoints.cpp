#include <functional>
#include "client.h"
#include "discord_object_interface.h"
#include "vector"

#if _MSC_VER && !__INTEL_COMPILER
#pragma warning( disable: 4458 ) //warns about variables that hide class members
#endif

//important note, all requests on sync mode throw on an http error

namespace SleepyDiscord {
	void BaseDiscordClient::testFunction(std::string teststring) {

	}
	//
	//channel functions
	//

	//ObjectResponse<Message> BaseDiscordClient::sendMessage(Snowflake<Channel> channelID, CreateMessageParams& params) {
	//	return ObjectResponse<Message>{ request(Post, path("channels/{channel.id}/messages", { channelID }), json::stringifyObj(params)) };
	//}

	std::string createMessageBody(std::string& message, Embed& embed, MessageReference& replyingTo, TTS tts) {
		rapidjson::Document doc;
		doc.SetObject();
		rapidjson::Value content;
		auto& allocator = doc.GetAllocator();
		content.SetString(message.c_str(), message.length());
		doc.AddMember("content", content, allocator);
		if (tts == TTS::EnableTTS) doc.AddMember("tts", true, allocator);
		if (!embed.empty()) doc.AddMember("embed", json::toJSON(embed, allocator), allocator);
		if (!replyingTo.messageID.empty()) doc.AddMember("message_reference", json::toJSON(embed, allocator), allocator);
		return json::stringify(doc);
	}

	ObjectResponse<Gateway> BaseDiscordClient::getGateway(RequestSettings<ObjectResponse<Gateway>> settings) {
		return ObjectResponse<Gateway>{ request(Get, "gateway/bot", settings) };
	}

	ObjectResponse<Message> BaseDiscordClient::sendMessage(Snowflake<Channel> channelID, std::string message, Embed embed, MessageReference replyingTo, TTS tts, RequestSettings<ObjectResponse<Message>> settings) {
		return ObjectResponse<Message>{ request(Post, path("channels/{channel.id}/messages", { channelID }), settings, createMessageBody(message, embed, replyingTo, tts)) };
	}

	ObjectResponse<Message> BaseDiscordClient::sendMessage(SendMessageParams params, RequestSettings<ObjectResponse<Message>> settings) {
		return ObjectResponse<Message>{ request(Post, path("channels/{channel.id}/messages", { params.channelID }), settings, json::stringifyObj(params)) };
	}

	ObjectResponse<Message> BaseDiscordClient::uploadFile(Snowflake<Channel> channelID, std::string fileLocation, std::string message, Embed embed, MessageReference replyingTo, RequestSettings<ObjectResponse<Message>> settings) {
		return ObjectResponse<Message>{
			request(Post, path("channels/{channel.id}/messages", { channelID }), settings, "", {
				{ "file", filePathPart{fileLocation} },
				{ "payload_json", createMessageBody(message, embed, replyingTo, TTS::DisableTTS) }
			})
		};
	}

	ObjectResponse<Message> BaseDiscordClient::uploadFile(SendMessageParams params, std::string fileLocation, RequestSettings<ObjectResponse<Message>> settings) {
		return ObjectResponse<Message>{
			request(Post, path("channels/{channel.id}/messages", { params.channelID }), settings, "", {
				{ "file", filePathPart{fileLocation} },
				{ "payload_json", json::stringifyObj(params) }
			})
		};
	}

	ObjectResponse<Message> BaseDiscordClient::editMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string newMessage, Embed embed, RequestSettings<ObjectResponse<Message>> settings) {
		MessageReference mr{};
		return ObjectResponse<Message>{ request(Patch, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }), settings, createMessageBody(newMessage, embed, mr, TTS::DisableTTS)) };
	}

	ObjectResponse<Message> BaseDiscordClient::editMessage(EditMessageParams params, RequestSettings<ObjectResponse<Message>> settings) {
		return ObjectResponse<Message>{ request(Patch, path("channels/{channel.id}/messages/{message.id}", { params.channelID, params.messageID }), settings, json::stringifyObj(params)) };
	}

	BoolResponse BaseDiscordClient::deleteMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::bulkDeleteMessages(Snowflake<Channel> channelID, std::vector<Snowflake<Message>> messageIDs, RequestSettings<BoolResponse> settings) {
		std::string JSON = "{\"messages\":[";
		for (Snowflake<Message> messageID : messageIDs) {
			JSON += messageID;
			JSON += ',';
		}
		if (messageIDs.size())
			JSON.pop_back();
		JSON += "]}";
		return { request(Post, path("channels/{channel.id}/messages/bulk-delete", { channelID }), settings, JSON), EmptyRespFn() };
	}

	ObjectResponse<Channel> BaseDiscordClient::editChannel(Snowflake<Channel> channelID, std::string name, std::string topic, RequestSettings<ObjectResponse<Channel>> settings) {
		rapidjson::Document doc;
		doc.SetObject();
		auto& allocator = doc.GetAllocator();
		rapidjson::Value nameValue;
		if (name != "") {
			nameValue.SetString(name.c_str(), name.length());
			doc.AddMember("name", nameValue, allocator);
		}
		rapidjson::Value topicValue;
		if (topic != "") {
			topicValue.SetString(topic.c_str(), topic.length());
			doc.AddMember("topic", topicValue, allocator);
		}
		return ObjectResponse<Channel>{ request(Patch, path("channels/{channel.id}", { channelID }), settings, json::stringify(doc)) };
	}

	ObjectResponse<Channel> BaseDiscordClient::editChannelName(Snowflake<Channel> channelID, std::string name, RequestSettings<ObjectResponse<Channel>> settings) {
		return editChannel(channelID, name, "", settings);
	}

	ObjectResponse<Channel> BaseDiscordClient::editChannelTopic(Snowflake<Channel> channelID, std::string topic, RequestSettings<ObjectResponse<Channel>> settings) {
		return editChannel(channelID, "", topic, settings);
	}

	ObjectResponse<Channel> BaseDiscordClient::deleteChannel(Snowflake<Channel> channelID, RequestSettings<ObjectResponse<Channel>> settings) {
		return ObjectResponse<Channel>{ request(Delete, path("channels/{channel.id}", { channelID }), settings) };
	}

	ObjectResponse<Channel> BaseDiscordClient::getChannel(Snowflake<Channel> channelID, RequestSettings<ObjectResponse<Channel>> settings) {
		return ObjectResponse<Channel>{ request(Get, path("channels/{channel.id}", { channelID }), settings) };
	}

	ArrayResponse<Message> BaseDiscordClient::getMessages(Snowflake<Channel> channelID, GetMessagesKey when, Snowflake<Message> messageID, uint8_t _limit, RequestSettings<ArrayResponse<Message>> settings) {
		const uint8_t trueLimit = 100 < _limit ? 100 : _limit;
		std::string key;
		switch (when) {
		case GetMessagesKey::around: key = "?around=" + messageID; break;
		case GetMessagesKey::before: key = "?before=" + messageID; break;
		case GetMessagesKey::after:  key = "?after=" + messageID; break;
		case GetMessagesKey::limit:  key = "?"                    ; break;
		default:     key = ""                     ; break;
		}
		if (trueLimit != 0 && when != GetMessagesKey::limit) key += '&';
		return ArrayResponse<Message>{
			request(Get,
				path("channels/{channel.id}/messages{key}{limit}", { channelID, key,
				(trueLimit != 0 ? "limit=" + std::to_string(trueLimit) : "") }), settings
			)
		};
	}

	ObjectResponse<Message> BaseDiscordClient::getMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<ObjectResponse<Message>> settings) {
		return ObjectResponse<Message>{ request(Get, path("channels/{channel.id}/messages/{message.id}", { channelID, messageID }), settings) };
	}

	std::string convertEmojiToURL(const std::string emoji) {
		if(emoji.empty() || emoji[0] == '%')
			return emoji; //no need to convert
		return escapeURL(emoji);
	}

	BoolResponse BaseDiscordClient::addReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, RequestSettings<BoolResponse> settings) {
		return { request(Put, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/@me", { channelID, messageID, convertEmojiToURL(emoji) }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::removeReaction(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, Snowflake<User> userID) {
		return { request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}/{user.id}", { channelID, messageID, convertEmojiToURL(emoji), userID })), EmptyRespFn() };
	}

	ArrayResponse<User> BaseDiscordClient::getReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID, std::string emoji, RequestSettings<ArrayResponse<Reaction>> settings) {
		return ArrayResponse<User>{ request(Get, path("channels/{channel.id}/messages/{message.id}/reactions/{emoji}", { channelID, messageID, convertEmojiToURL(emoji) }), settings) };
	}

	StandardResponse BaseDiscordClient::removeAllReactions(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<StandardResponse> settings) {
		return StandardResponse{ request(Delete, path("channels/{channel.id}/messages/{message.id}/reactions", { channelID, messageID }), settings) };
	}

	BoolResponse BaseDiscordClient::editChannelPermissions(Snowflake<Channel> channelID, Snowflake<Overwrite> overwriteID, int allow, int deny, std::string type) {
		return { request(
			Put,
			path("channels/{channel.id}/permissions/{overwrite.id}", { channelID, overwriteID }),
			json::createJSON({
				{ "allow", std::to_string(allow) },
				{ "deny", std::to_string(deny) },
				{ "type", json::string(type) }
		})), EmptyRespFn() };
	}

	ArrayResponse<Invite> BaseDiscordClient::getChannelInvites(Snowflake<Channel> channelID, RequestSettings<ArrayResponse<Invite>> settings) {
		return ArrayResponse<Invite>{ request(Get, path("channels/{channel.id}/invites", { channelID }), settings) };
	}

	ObjectResponse<Invite> BaseDiscordClient::createChannelInvite(Snowflake<Channel> channelID, const uint64_t maxAge, const uint64_t maxUses, const bool temporary, const bool unique) {
		return ObjectResponse<Invite>{
			request(Post, path("channels/{channel.id}/invites", { channelID }),
				json::createJSON({
					{"max_age", json::optionalUInteger(maxAge) },
					{"max_uses", json::optionalUInteger(maxUses) },
					{ "temporary", temporary ? "true" : ""},
					{ "unique", unique ? "true" : ""}
				}
			))
		};
	}

	BoolResponse BaseDiscordClient::removeChannelPermission(Snowflake<Channel> channelID, std::string ID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("channels/{channel.id}/permissions/{overwrite.id}", { channelID, ID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::sendTyping(Snowflake<Channel> channelID, RequestSettings<BoolResponse> settings) {
		return { request(Post, path("channels/{channel.id}/typing", { channelID }), settings), EmptyRespFn() };
	}

	ArrayResponse<Message> BaseDiscordClient::getPinnedMessages(Snowflake<Channel> channelID, RequestSettings<ArrayResponse<Message>> settings) {
		return ArrayResponse<Message>{ request(Get, path("channels/{channel.id}/pins", { channelID }), settings) };
	}

	BoolResponse BaseDiscordClient::pinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<BoolResponse> settings) {
		return { request(Put, path("channels/{channel.id}/pins/{message.id}", { channelID, messageID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::unpinMessage(Snowflake<Channel> channelID, Snowflake<Message> messageID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("channels/{channel.id}/pins/{message.id}", { channelID, messageID }), settings), EmptyRespFn() };
	}

	StandardResponse BaseDiscordClient::addRecipient(Snowflake<Channel> channelID, Snowflake<User> userID, RequestSettings<StandardResponse> settings) {
		return StandardResponse{ request(Put, path("channels/{channel.id}/recipients/{user.id}", { channelID, userID }), settings) };
	}

	StandardResponse BaseDiscordClient::removeRecipient(Snowflake<Channel> channelID, Snowflake<User> userID, RequestSettings<StandardResponse> settings) {
		return StandardResponse{ request(Delete, path("channels/{channel.id}/recipients/{user.id}", { channelID, userID }), settings) };
	}

	//
	//server functions
	//
	ObjectResponse<Channel> BaseDiscordClient::createTextChannel(Snowflake<Server> serverID, std::string name, RequestSettings<ObjectResponse<Channel>> settings) {
		return ObjectResponse<Channel>{
			request(Post, path("guilds/{guild.id}/channels", { serverID }), settings, "{\"name\": " + json::string(name) + ", \"type\": 0}")
		};
	}

	ObjectResponse<Channel> BaseDiscordClient::createChannel(Snowflake<Server> serverID, std::string name, Channel::ChannelType ChannelType, RequestSettings<ObjectResponse<Channel>> settings) {
		return ObjectResponse<Channel>{
			request(Post, path("guilds/{guild.id}/channels", { serverID }), settings, "{\"name\": " + json::string(name) + ", \"type\": "+ std::to_string(ChannelType) +"}")
		};
	}

	ArrayResponse<Channel> BaseDiscordClient::editChannelPositions(Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions, RequestSettings<ArrayResponse<Channel>> settings) {
		return ArrayResponse<Channel>{ request(Patch, path("guilds/{guild.id}/channels", { serverID }), getEditPositionString(positions)) };
	}

	ObjectResponse<ServerMember> SleepyDiscord::BaseDiscordClient::getMember(Snowflake<Server> serverID, Snowflake<User> userID, RequestSettings<ObjectResponse<ServerMember>> settings) {
		return ObjectResponse<ServerMember>{ request(Get, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), settings) };
	}

	ArrayResponse<ServerMember> BaseDiscordClient::listMembers(Snowflake<Server> serverID, uint16_t limit, std::string after, RequestSettings<ArrayResponse<ServerMember>> settings) {
		//Todo better Query String Params support
		std::string limitParm = limit != 0 ? "?limit=" + std::to_string(limit) : "";
		std::string afterParm = after != "" ? "after=" + after : "";
		if (afterParm != "" && limitParm != "") limitParm += '&';
		else if (afterParm != "" && limitParm == "") limitParm += '?';
		return ArrayResponse<ServerMember>{ request(Get, path("guilds/{guild.id}/members{limit}{after}", { serverID, limitParm, afterParm }), settings) };
	}

	ObjectResponse<ServerMember> BaseDiscordClient::addMember(Snowflake<Server> serverID, Snowflake<User> userID, std::string accesToken, std::string nick, std::vector<Role> roles, bool mute, bool deaf) {
		std::string rolesString = "";
		if (roles.empty()){
			rolesString = "";
		} else {
			int i = -1;
			std::vector<std::string> values(roles.size());
			for (Role role : roles) {
				values[++i] = json::createJSON({
					{ "id"         , json::string  (role.ID         ) },
					{ "name"       , json::string  (role.name       ) },
					{ "color"      , json::UInteger(role.color      ) },
					{ "hoist"      , json::boolean (role.hoist      ) },
					{ "position"   , json::integer (role.position   ) },
					{ "managed"    , json::boolean (role.managed    ) },
					{ "mentionable", json::boolean (role.mentionable) }
				});
			}
			rolesString = json::createJSONArray(values);
		}

		return ObjectResponse<ServerMember>{
			request(Put, path("guilds/{guild.id}/members/{user.id}"), json::createJSON({
				{ "access_token", json::string (accesToken) },
				{ "nick"        , json::string (nick)       },
				{ "roles"       , rolesString               },
				{ "mute"        , json::boolean(mute)       },
				{ "deaf"        , json::boolean(deaf)       }
			})) 
		};
	}

	BoolResponse BaseDiscordClient::editMember(Snowflake<Server> serverID, Snowflake<User> userID, std::string nickname, std::vector<Snowflake<Role>> roles, int8_t mute, int8_t deaf, Snowflake<Channel> channelID) {
		const std::string muteString = mute != -1 ? json::boolean(mute) : "";
		const std::string deafString = deaf != -1 ? json::boolean(deaf) : "";

		return { request(Patch, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), json::createJSON({
			{ "nick"      , json::string(nickname)       },
			{ "roles"     , json::createJSONArray(roles) },
			{ "mute"      , muteString                   },
			{ "deaf"      , deafString                   },
			{ "channel_id", json::string(channelID)      },
		})), EmptyRespFn() };
	}

	ArrayResponse<Role> BaseDiscordClient::editRolePosition(Snowflake<Server> serverID, std::vector<std::pair<std::string, uint64_t>> positions, RequestSettings<ArrayResponse<Role>> settings) {
		return ArrayResponse<Role>{ request(Patch, path("guilds/{guild.id}/roles", { serverID }), settings, getEditPositionString(positions)) };
	}


	StringResponse BaseDiscordClient::editRole(Snowflake<Server> serverID, Snowflake<Role> roleID, std::string name, Permission permissions, uint32_t color, int8_t hoist, int8_t mentionable) {
		const std::string colorString       = color       >> 24 == 0 ? std::to_string(color      ) : "";	//if over 24 bits, do not change color
		const std::string hoistString       = hoist       >> 1  == 0 ? json::boolean (hoist      ) : "";	//if larger then 1 bit, do change hoist
		const std::string mentionableString = mentionable >> 1  == 0 ? json::boolean (mentionable) : "";

		return StringResponse{
			request(Patch, path("guilds/{guild.id}/roles/{role.id}", { serverID, roleID }), json::createJSON({
				{ "name"       , json::string(name)         },
				{ "permissions", json::integer(permissions) },
				{ "color"      , colorString                },
				{ "hoist"      , hoistString                },
				{ "mentionable", mentionableString          }
			}))
		};
	}

	BoolResponse SleepyDiscord::BaseDiscordClient::deleteRole(Snowflake<Server> serverID, Snowflake<Role> roleID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/roles/{role.id}", { serverID, roleID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::muteServerMember(Snowflake<Server> serverID, Snowflake<User> userID, bool mute, RequestSettings<BoolResponse> settings) {
		return { request(Patch, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), settings, mute ? "{\"mute\":true}" : "{\"mute\":false}"), EmptyRespFn() };
	}

	//needs ablily to turn channel into json
	/*ObjectResponse<Server> BaseDiscordClient::createServer(std::string name, std::string region, std::string icon, int verificationLevel, int defaultMessageNotifications, int explicitContentLevel, std::vector<Role> roles, std::vector<Channel> channels) {
		request(Post, "guilds", json::createJSON({
			{ "name"                         , json::string (name) },
			{ "region"                       , json::string(region) },
			{ "icon"                         , json::string(icon) },
			{ "verification_level"           , json::integer(verificationLevel) },
			{ "default_message_notifications", json::integer(defaultMessageNotifications) },
			{ "explicit_content_filter"      , json::integer(explicitContentLevel) },
			{ "roles"                        , json::createJSONArray(roles) },
			{ "channels"                     , json::createJSONArray(channels) }
		}));
	}*/

	ObjectResponse<Server> BaseDiscordClient::getServer(Snowflake<Server> serverID, RequestSettings<ObjectResponse<Server>> settings) {
		return ObjectResponse<Server>{ request(Get, path("guilds/{guild.id}", { serverID }), settings) };
	}

	ObjectResponse<Server> BaseDiscordClient::deleteServer(Snowflake<Server> serverID, RequestSettings<ObjectResponse<Server>> settings) {
		return ObjectResponse<Server>{ request(Delete, path("guilds/{guild.id}", { serverID }), settings) };
	}

	ArrayResponse<Channel> SleepyDiscord::BaseDiscordClient::getServerChannels(Snowflake<Server> serverID, RequestSettings<ArrayResponse<Channel>> settings) {
		return ArrayResponse<Channel>{ request(Get, path("guilds/{guild.id}/channels", { serverID }), settings) };
	}

	BoolResponse BaseDiscordClient::editNickname(Snowflake<Server> serverID, std::string newNickname, RequestSettings<BoolResponse> settings) {
		return { request(Patch, path("guilds/{guild.id}/members/@me/nick", { serverID }), settings, "{\"nick\":" + json::string(newNickname) + "}"), StandardRespFn() };
	}

	BoolResponse BaseDiscordClient::addRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID, RequestSettings<BoolResponse> settings) {
		return { request(Put, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", { serverID, userID, roleID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::removeRole(Snowflake<Server> serverID, Snowflake<User> userID, Snowflake<Role> roleID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/members/{user.id}/roles/{role.id}", { serverID, userID, roleID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::kickMember(Snowflake<Server> serverID, Snowflake<User> userID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/members/{user.id}", { serverID, userID }), settings), EmptyRespFn() };
	}

	ArrayResponse<User> BaseDiscordClient::getBans(Snowflake<Server> serverID, RequestSettings<ArrayResponse<User>> settings) {
		return ArrayResponse<User>{ request(Get, path("guilds/{guild.id}/bans", { serverID }), settings) };
	}

	BoolResponse BaseDiscordClient::banMember(Snowflake<Server> serverID, Snowflake<User> userID, int deleteMessageDays, std::string reason, RequestSettings<BoolResponse> settings) {
		rapidjson::Document doc;
		doc.SetObject();
		auto& allocator = doc.GetAllocator();
		if (deleteMessageDays == -1)
			doc.AddMember("delete_message_days", deleteMessageDays, allocator);
		if (!reason.empty()) {
			rapidjson::Value reasonValue;
			reasonValue.SetString(reason.c_str(), reason.length());
			doc.AddMember("reason", reasonValue, allocator);
		}
		return { request(Put, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID }), settings, json::stringify(doc)), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::unbanMember(Snowflake<Server> serverID, Snowflake<User> userID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/bans/{user.id}", { serverID, userID }), settings), EmptyRespFn() };
	}

	ArrayResponse<Role> BaseDiscordClient::getRoles(Snowflake<Server> serverID, RequestSettings<ArrayResponse<Role>> settings) {
		return ArrayResponse<Role>{ request(Get, path("guilds/{guild.id}/roles", { serverID }), settings) };
	}

	ObjectResponse<Role> BaseDiscordClient::createRole(Snowflake<Server> serverID, std::string name, Permission permissions, unsigned int color, bool hoist, bool mentiionable) {
		return ObjectResponse<Role>{
			request(Post, path("guilds/{guild.id}/roles", { serverID }), json::createJSON({
				{ "name"       , json::string (name        ) },
				{ "permissions", json::integer(permissions ) },
				{ "color"      , json::integer(color       ) },
				{ "hoist"      , json::boolean(hoist       ) },
				{ "mentionable", json::boolean(mentiionable) }
			}))
		};
	}

	StandardResponse BaseDiscordClient::pruneMembers(Snowflake<Server> serverID, const unsigned int numOfDays, RequestSettings<StandardResponse> settings) {
		if (numOfDays == 0) return StandardResponse{ BAD_REQUEST };
		return StandardResponse{ request(Post, path("guilds/{guild.id}/prune", { serverID }), settings, "{\"days\":" + std::to_string(numOfDays) + '}') };
	}

	ArrayResponse<VoiceRegion> BaseDiscordClient::getVoiceRegions(RequestSettings<ArrayResponse<VoiceRegion>> settings) {
		return ArrayResponse<VoiceRegion>{ request(Get, path("guilds/{guild.id}/regions"), settings) };
	}

	ArrayResponse<Invite> BaseDiscordClient::getServerInvites(Snowflake<Server> serverID, RequestSettings<ArrayResponse<Invite>> settings) {
		return ArrayResponse<Invite>{ request(Get, path("guilds/{guild.id}/invites", { serverID }), settings) };
	}

	StringResponse BaseDiscordClient::getIntegrations(Snowflake<Server> serverID, RequestSettings<StringResponse> settings) {
		return StringResponse{ request(Get, path("guilds/{guild.id}/integrations", { serverID }), settings) };
	}

	BoolResponse BaseDiscordClient::createIntegration(Snowflake<Server> serverID, std::string type, std::string integrationID, RequestSettings<BoolResponse> settings) {
		return { request(Post, path("guilds/{guild.id}/integrations", { serverID }), settings, json::createJSON({
			{ "type", json::string(type) },
			{ "id", json::string(integrationID) }
		})), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::editIntergration(Snowflake<Server> serverID, std::string integrationID, int expireBegavior, int expireGracePeriod, bool enbleEmoticons) {
		return { request(Patch, path("guilds/{guild.id}/integrations/{integration.id}", { serverID, integrationID }), json::createJSON({
			{ "expire_behavior", json::integer(expireBegavior) },
			{ "expire_grace_period", json::integer(expireGracePeriod) },
			{ "enable_emoticons", json::boolean(enbleEmoticons) }
		})), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::deleteIntegration(Snowflake<Server> serverID, std::string integrationID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("guilds/{guild.id}/integrations/{integration.id}", { serverID, integrationID }), settings), EmptyRespFn() };
	}

	BoolResponse BaseDiscordClient::syncIntegration(Snowflake<Server> serverID, std::string integrationID, RequestSettings<BoolResponse> settings) {
		return { request(Post, path("guilds/{guild.id}/integrations/{integration.id}/sync", { serverID, integrationID }), settings), EmptyRespFn() };
	}

	ObjectResponse<ServerWidget> BaseDiscordClient::getServerWidget(Snowflake<Server> serverID, RequestSettings<ObjectResponse<ServerWidget>> settings) {
		return ObjectResponse<ServerWidget>{ request(Get, path("guilds/{guild.id}/widget", { serverID }), settings) };
	}

	//
	//Invite functions
	//
	ObjectResponse<Invite> BaseDiscordClient::inviteEndpoint(RequestMethod method, std::string inviteCode, RequestSettings<ObjectResponse<Invite>> settings) {
		return ObjectResponse<Invite>{ request(method, path("invites/{invite.code}", { inviteCode }), settings) };
	}

	ObjectResponse<Invite> BaseDiscordClient::getInvite(std::string inviteCode, RequestSettings<ObjectResponse<Invite>> settings) {
		return inviteEndpoint(Get, inviteCode, settings);
	}

	ObjectResponse<Invite> BaseDiscordClient::deleteInvite(std::string inviteCode, RequestSettings<ObjectResponse<Invite>> settings) {
		return inviteEndpoint(Delete, inviteCode, settings);
	}

	ObjectResponse<Invite> BaseDiscordClient::acceptInvite(std::string inviteCode, RequestSettings<ObjectResponse<Invite>> settings) {
		return inviteEndpoint(Post, inviteCode, settings);
	}
	//
	//User functions
	//
	ObjectResponse<User> BaseDiscordClient::getCurrentUser(RequestSettings<ObjectResponse<User>> settings) {
		return ObjectResponse<User>{ request(Get, "users/@me", settings) };
	}

	ObjectResponse<User> BaseDiscordClient::getUser(Snowflake<User> userID, RequestSettings<ObjectResponse<User>> settings) {
		return ObjectResponse<User>{ request(Get, path("users/{user.id}", { userID }), settings) };
	}

	ArrayResponse<Server> BaseDiscordClient::getServers(RequestSettings<ArrayResponse<Server>> settings) {
		return ArrayResponse<Server>{ request(Get, "users/@me/guilds", settings) };
	}

	BoolResponse BaseDiscordClient::leaveServer(Snowflake<Server> serverID, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path("users/@me/guilds/{guild.id}", { serverID }), settings), EmptyRespFn() };
	}

	ArrayResponse<Channel> BaseDiscordClient::getDirectMessageChannels(RequestSettings<ArrayResponse<Channel>> settings) {
		return ArrayResponse<Channel>{ request(Get, "users/@me/channels", settings) };
	}

	ObjectResponse<Channel> BaseDiscordClient::createDirectMessageChannel(std::string recipientID, RequestSettings<ObjectResponse<Channel>> settings) {
		return ObjectResponse<Channel>{ request(Post, "users/@me/channels", settings, json::createJSON({ { "recipient_id", recipientID } })) };
	}

	ArrayResponse<Connection> BaseDiscordClient::getUserConnections(RequestSettings<ArrayResponse<Connection>> settings) {
		return ArrayResponse<Connection>{ request(Get, "users/@me/connections", settings) };
	}

	//
	//Webhook functions
	//
	ObjectResponse<Webhook> BaseDiscordClient::createWebhook(Snowflake<Channel> channelID, std::string name, std::string avatar, RequestSettings<ObjectResponse<Webhook>> settings) {
		return ObjectResponse<Webhook>{ request(Post, path("channels/{channel.id}/webhooks", { channelID }), settings, json::createJSON({
			{"name", json::string(name)},
			{"avatar", json::string(avatar)}
		})) };
	}

	ArrayResponse<Webhook> BaseDiscordClient::getChannelWebhooks(Snowflake<Channel> channelID, RequestSettings<ArrayResponse<Webhook>> settings) {
		return ArrayResponse<Webhook>{ request(Get, path("channels/{channel.id}/webhooks", { channelID }), settings) };
	}

	ArrayResponse<Webhook> BaseDiscordClient::getServerWebhooks(Snowflake<Server> serverID, RequestSettings<ArrayResponse<Webhook>> settings) {
		return ArrayResponse<Webhook>{ request(Get, path("guilds/{guild.id}/webhooks", { serverID }), settings) };
	}

	inline const char* optionalWebhookToken(std::string webhookToken) {
		return webhookToken != "" ? "webhooks/{webhook.id}/{webhook.token}" : "webhooks/{webhook.id}";
	}

	ObjectResponse<Webhook> BaseDiscordClient::getWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, RequestSettings<ObjectResponse<Webhook>> settings) {
		return ObjectResponse<Webhook>{ request(Get, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }), settings) };
	}

	ObjectResponse<Webhook> BaseDiscordClient::editWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::string name, std::string avatar) {
		return ObjectResponse<Webhook>{ request(Patch, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }), json::createJSON({
			{ "name", json::string(name) },
			{ "avatar", json::string(avatar) }
		})) };
	}

	BoolResponse BaseDiscordClient::deleteWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, RequestSettings<BoolResponse> settings) {
		return { request(Delete, path(optionalWebhookToken(webhookToken), { webhookID, webhookToken }), settings), EmptyRespFn() };
	}

	//excute webhook

	ObjectResponse<Webhook> BaseDiscordClient::requestExecuteWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::pair<std::string, std::string> pair, bool wait, std::string username, std::string avatar_url, bool tts) {
		return ObjectResponse<Webhook>{
			request(Post, path("webhooks/{webhook.id}/{webhook.token}{wait}", { webhookID, webhookToken, (wait ? "?around=true" : "") }), json::createJSON({
				pair,
				{ "username"  , json::string(username  ) },
				{ "avatar_url", json::string(avatar_url) },
				{ "tts"       , (tts ? "true" : "")      }
			})) };
	}

	ObjectResponse<Webhook> BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::string content, bool wait, std::string username, std::string avatar_url, bool tts) {
		return ObjectResponse<Webhook>{ requestExecuteWebhook(webhookID, webhookToken, { "content", json::string(content) }, wait, username, avatar_url, tts) };
	}

	//Webhook SleepyDiscord::BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, std::vector<Embed> embeds, bool wait, std::string username, std::string avatar_url bool tts) {
	//	
	//	return requestExecuteWebhook(webhookID, webhookToken, { "embeds", crazy stuff happens here }, wait, username, avatar_url, tts);
	//}
	
	ObjectResponse<Webhook> BaseDiscordClient::executeWebhook(Snowflake<Webhook> webhookID, std::string webhookToken, filePathPart file, bool /*wait*/, std::string username, std::string avatar_url, bool tts) {
		return ObjectResponse<Webhook>{ request(Post, path("webhooks/{webhook.id}/{webhook.token}", { webhookID, webhookToken }), "", {
			{ "file"      , filePathPart(file)  },
			{ "username"  , username            },
			{ "avatar_url", avatar_url          },
			{ "tts"       , (tts ? "true" : "") }
		}) };
	}

	ArrayResponse<AppCommand> BaseDiscordClient::getGlobalAppCommands(Snowflake<DiscordObject>::RawType applicationID, RequestSettings<ArrayResponse<AppCommand>> settings) {
		return ArrayResponse<AppCommand>{ request(Get, path("applications/{application.id}/commands", { applicationID }), settings) };
	}

	ObjectResponse<AppCommand> BaseDiscordClient::getGlobalAppCommand(
		Snowflake<DiscordObject>::RawType applicationID, Snowflake<AppCommand> commandID,
		RequestSettings<ObjectResponse<AppCommand>> settings
	) {
		return ObjectResponse<AppCommand>{ request(Get, path("applications/{application.id}/commands/{command.id}", { applicationID, commandID }), settings) };
	}

	BoolResponse BaseDiscordClient::deleteGlobalAppCommand(
		Snowflake<DiscordObject>::RawType applicationID, Snowflake<AppCommand> commandID, RequestSettings<BoolResponse> settings
	) {
		return { request(Delete, path("applications/{application.id}/commands/{command.id}", { applicationID, commandID }), settings), EmptyRespFn() };
	}

	ArrayResponse<AppCommand> BaseDiscordClient::getServerAppCommands(
		Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, RequestSettings<ArrayResponse<AppCommand>> settings
	) {
		return ArrayResponse<AppCommand>{ request(Get, path("applications/{application.id}/guilds/{guild.id}/commands", { applicationID, serverID }), settings) };
	}

	ObjectResponse<AppCommand> BaseDiscordClient::getServerAppCommand(
		Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, Snowflake<AppCommand> commandID,
		RequestSettings<ObjectResponse<AppCommand>> settings
	) {
		return ObjectResponse<AppCommand>{ request(Get, path("applications/{application.id}/guilds/{guild.id}/commands/{command.id}", { applicationID, serverID, commandID }), settings) };
	}

	BoolResponse BaseDiscordClient::deleteServerAppCommand(
		Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, Snowflake<AppCommand> commandID, RequestSettings<BoolResponse> settings
	) {
		return { request(Delete, path("applications/{application.id}/guilds/{guild.id}/commands/{command.id}", { applicationID, serverID, commandID }), settings), EmptyRespFn() };
	}

	ObjectResponse<Message> BaseDiscordClient::editOriginalInteractionResponse(
		Snowflake<DiscordObject>::RawType applicationID, std::string interactionToken, EditWebhookParams params, RequestSettings<BoolResponse> settings
	) {
		return ObjectResponse<Message>{ request(Patch, path("webhooks/{application.id}/{interaction.token}/messages/@original", { applicationID, interactionToken }), settings, json::stringifyObj(params)) };
	}

	BoolResponse BaseDiscordClient::deleteOriginalInteractionResponse(
		Snowflake<DiscordObject>::RawType applicationID, std::string interactionToken, RequestSettings<BoolResponse> settings
	) {
		return { request(Delete, path("webhooks/{application.id}/{interaction.token}/messages/@original", { applicationID, interactionToken }), settings), EmptyRespFn() };
	}

	ObjectResponse<Message> BaseDiscordClient::createFollowupMessage(
		Snowflake<DiscordObject>::RawType applicationID, std::string interactionToken, FollowupMessage params, RequestSettings<BoolResponse> settings
	) {
		return ObjectResponse<Message>{ request(Post, path("webhooks/{application.id}/{interaction.token}", { applicationID, interactionToken }), settings, json::stringifyObj(params)) };
	}

	ObjectResponse<Message> BaseDiscordClient::editFollowupMessage(
		Snowflake<DiscordObject>::RawType applicationID, std::string interactionToken, Snowflake<Message> messageID, EditWebhookParams params, RequestSettings<BoolResponse> settings
	) {
		return ObjectResponse<Message>{ request(Patch, path("webhooks/{application.id}/{interaction.token}/messages/{message.id}", { applicationID, interactionToken, messageID }), settings, json::stringifyObj(params)) };
	}

	BoolResponse BaseDiscordClient::deleteFollowupMessage(
		Snowflake<DiscordObject>::RawType applicationID, std::string interactionToken, Snowflake<Message> messageID, RequestSettings<BoolResponse> settings
	) {
		return { request(Delete, path("webhooks/{application.id}/{interaction.token}/messages/{message.id}", { applicationID, interactionToken, messageID }), settings), EmptyRespFn() };
	}
	/// <summary>
	/// Batch edits permissions for all commands in a guild. Takes an array of partial objects including id and permissions.
	/// https://discord.com/developers/docs/interactions/slash-commands#batch-edit-application-command-permissions
	/// </summary>
	BoolResponse BaseDiscordClient::batchEditAppCommandPermissions(
		Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, std::vector<ServerAppCommandPermissions> permissions, RequestSettings<BoolResponse> settings
	) {
		rapidjson::Document doc;
		doc.SetObject();
		auto& allocator = doc.GetAllocator();
		for (auto& command : permissions) {
			doc.AddMember("id", command.ID.number(), allocator);
			rapidjson::Value arr{ rapidjson::Type::kArrayType };
			for (auto& permission : command.permissions) {
				arr.PushBack(json::toJSON(permission, allocator), allocator);
			}
			doc.AddMember("permissions", arr, allocator);
		}
		return BoolResponse{ request(Put, path("applications/{application.id}/guilds/{guild.id}/commands/permissions", { applicationID, serverID }), settings , json::stringify(doc)) };
	}
	/// <summary>
	/// Edits command permissions for a specific command for your application in a guild.
	/// https://discord.com/developers/docs/interactions/slash-commands#edit-application-command-permissions
	/// </summary>
	BoolResponse BaseDiscordClient::editServerAppCommandPermission(
		Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, Snowflake<AppCommand> commandID, std::vector<AppCommand::Permissions> permissions, RequestSettings<BoolResponse> settings
	) {
		rapidjson::Document doc;
		doc.SetObject();
		auto& allocator = doc.GetAllocator();
		rapidjson::Value arr{ rapidjson::Type::kArrayType };
		for (auto& permission : permissions) {
			arr.PushBack(json::toJSON(permission, allocator), allocator);
		}
		doc.AddMember("permissions", arr, allocator);
		return BoolResponse{ request(Put, path("applications/{application.id}/guilds/{guild.id}/commands/{command.id}/permissions", { applicationID, serverID, commandID }), settings , json::stringify(doc)) };
	}
	/// <summary>
	/// Fetches command permissions for all commands for your application in a guild.
	/// https://discord.com/developers/docs/interactions/slash-commands#get-guild-application-command-permissions
	/// </summary>
	ArrayResponse<ServerAppCommandPermissions> BaseDiscordClient::getServerAppCommandPermissions(
		Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, RequestSettings<ArrayResponse<ServerAppCommandPermissions>> settings
	) {
		return ArrayResponse<ServerAppCommandPermissions>{ request(Get, path("applications/{application.id}/guilds/{guild.id}/commands/permissions", { applicationID, serverID }), settings) };
	}
	/// <summary>
	/// Fetches command permissions for a specific command for your application in a guild.
	/// https://discord.com/developers/docs/interactions/slash-commands#get-application-command-permissions
	/// </summary>
	ObjectResponse<ServerAppCommandPermissions> BaseDiscordClient::getAppCommandPermissions(
		Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, Snowflake<AppCommand> commandID, RequestSettings<ObjectResponse<ServerAppCommandPermissions>> settings
	) {
		return ObjectResponse<ServerAppCommandPermissions>{ request(Get, path("applications/{application.id}/guilds/{guild.id}/commands/{command.id}/permissions", { applicationID, serverID, commandID }), settings) };
	}

	ArrayResponse<AppCommand> BaseDiscordClient::getAppCommands(Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, RequestSettings<ArrayResponse<AppCommand>> settings) {
		if (serverID.empty()) return getGlobalAppCommands(applicationID, settings);
		return getServerAppCommands(applicationID, serverID, settings);
	}

	ObjectResponse<AppCommand> BaseDiscordClient::getAppCommand(Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, Snowflake<AppCommand> commandID, RequestSettings<ObjectResponse<AppCommand>> settings) {
		if (serverID.empty()) return getGlobalAppCommand(applicationID, commandID, settings);
		return getServerAppCommand(applicationID, serverID, commandID,  settings);
	}

	BoolResponse BaseDiscordClient::deleteAppCommand(Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, Snowflake<AppCommand> commandID, RequestSettings<BoolResponse> settings) {
		if (serverID.empty()) return deleteGlobalAppCommand(applicationID, commandID, settings);
		return deleteServerAppCommand(applicationID, serverID, commandID, settings);
	}

	BoolResponse BaseDiscordClient::bulkOverwriteServerAppCommands(Snowflake<DiscordObject>::RawType applicationID, Snowflake<Server> serverID, std::vector<AppCommand> commands, RequestSettings<BoolResponse> settings) {
		rapidjson::Document doc;
		doc.SetArray();
		auto& allocator = doc.GetAllocator();
		for (auto& command : commands) {
			doc.PushBack(json::toJSON(command, allocator), allocator);
		}
		return BoolResponse{ request(Put, path("applications/{application.id}/guilds/{guild.id}/commands", {applicationID, serverID}), settings, json::stringify(doc)) };
	}

	BoolResponse BaseDiscordClient::bulkOverwriteGlobalAppCommands(Snowflake<DiscordObject>::RawType applicationID, std::vector<AppCommand> commands, RequestSettings<BoolResponse> settings) {
		rapidjson::Document doc;
		doc.SetArray();
		auto& allocator = doc.GetAllocator();
		for (auto& command : commands) {
			doc.PushBack(json::toJSON(command, allocator), allocator);
		}
		return BoolResponse{ request(Put, path("applications/{application.id}/commands", {applicationID}), settings, json::stringify(doc)) };
	}

	ObjectResponse<User> BaseDiscordClient::createStageInstance(Snowflake<Channel> channelID, std::string topic, StageInstance::PrivacyLevel privacyLevel, RequestSettings<ObjectResponse<User>> settings) {
		rapidjson::Document doc;
		doc.SetObject();
		auto& allocator = doc.GetAllocator();
		const std::string& channelIDStr = channelID.string();
		doc.AddMember("channel_id", rapidjson::Value::StringRefType{ channelIDStr.c_str(), channelIDStr.length() }, allocator);
		doc.AddMember("topic", rapidjson::Value::StringRefType{ topic.c_str(), topic.length() }, allocator);
		if (privacyLevel != StageInstance::PrivacyLevel::NotSet)
			doc.AddMember("privacy_level", static_cast<StageInstance::PrivacyLevelRaw>(privacyLevel), allocator);
		return ObjectResponse<User>{
			request(Post, path("/stage-instances", {}), settings, json::stringify(doc))
		};
	}

	ObjectResponse<StageInstance> BaseDiscordClient::getStageInstance(Snowflake<Channel> channelID, RequestSettings<ObjectResponse<StageInstance>> settings) {
		return ObjectResponse<StageInstance>{ request(Get, path("/stage-instances/{channel.id}", { channelID }), settings)};
	}

	BoolResponse BaseDiscordClient::editStageInstance(Snowflake<Channel> channelID, std::string topic, StageInstance::PrivacyLevel privacyLevel, RequestSettings<BoolResponse> settings) {
		rapidjson::Document doc;
		doc.SetObject();
		auto& allocator = doc.GetAllocator();
		doc.AddMember("topic", rapidjson::Value::StringRefType{ topic.c_str(), topic.length() }, allocator);
		if (privacyLevel != StageInstance::PrivacyLevel::NotSet)
			doc.AddMember("privacy_level", static_cast<StageInstance::PrivacyLevelRaw>(privacyLevel), allocator);
		return BoolResponse{ request(Patch, path("/stage-instances/{channel.id}", {channelID}), settings, json::stringify(doc)) };
	}

	BoolResponse BaseDiscordClient::deleteStageInstance(Snowflake<Channel> channelID, RequestSettings<BoolResponse> settings) {
		return BoolResponse{ request(Delete, path("/stage-instances/{channel.id}", {channelID}), settings) };
	}

	std::string CDN_path(const std::initializer_list<std::string> path) {
		static constexpr auto CDN_URL = BaseDiscordClient::getCDN_URL();
		std::size_t pathLength = CDN_URL.length();
		for (const std::string& str : path) {
			pathLength += str.length();
		}
		std::string CDN_path;
		CDN_path.reserve(pathLength);
		CDN_path += CDN_URL.data();
		for (const std::string& str : path) {
			CDN_path += str;
		}
		return CDN_path;
	}

	//CDN
	void BaseDiscordClient::getServerBanner(Snowflake<Server> serverID, std::string banner, std::string format, std::function<void(StandardResponse&)> callback) {
		static constexpr const char* pathMid = "banners/";
		const std::string path = CDN_path({pathMid, serverID, "/", banner, format});
		postTask([path, callback]() {
			Session session;
			session.setUrl(path);
			auto response = StandardResponse{session.request(Get)};
			callback(response);
		});
	}
}