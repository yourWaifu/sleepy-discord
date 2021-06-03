#pragma once
#include <vector>
#include "discord_object_interface.h"
#include "snowflake.h"
#include "server.h"
#include "channel.h"
#include "embed.h"
#include "message.h"
#include "user.h"

namespace SleepyDiscord {
	
	struct AppCommand : public IdentifiableDiscordObject<AppCommand> {
		AppCommand() = default;
		AppCommand(json::Value & json);
		AppCommand(const nonstd::string_view & json);
		
		struct Option : public DiscordObject {
			Option() = default;
			Option(json::Value & json);
			Option(const nonstd::string_view & json);

			enum class Type : int {
				NONE              = 0, //made up type
				SUB_COMMAND       = 1,
				SUB_COMMAND_GROUP = 2,
				STRING            = 3,
				INTEGER	          = 4,
				BOOLEAN	          = 5,
				USER              = 6,
				CHANNEL           = 7,
				ROLE              = 8
			};

			struct Choice : public DiscordObject {
				Choice() = default;
				Choice(json::Value & json);
				Choice(const nonstd::string_view & json);

				std::string name;
				json::Value value;

				//warning crashes if type isn't correct
				template<class Type>
				inline const Type get() {
					return json::ClassTypeHelper<Type>::toType(value);
				}

				template<class Type>
				inline const bool get(Type& target) {
					return json::castValue<Type, json::ClassTypeHelper<Type>>(target, value);
				}

				template<class Type>
				inline void set(Type& val) {
					value = json::ClassTypeHelper<Type>::fromType(val);
				}

				JSONStructStart
					std::make_tuple(
						json::pair(&AppCommand::Option::Choice::name , "name" , json::REQUIRIED_FIELD),
						json::pair(&AppCommand::Option::Choice::value, "value", json::REQUIRIED_FIELD)
					);
				JSONStructEnd
			};

			Type type;
			std::string name;
			std::string description;
			enum class Default : char {
				True = true,
				False = false,
				Undefined = -1
			};
			Default isDefault = Default::Undefined; //default is an already taken keyword
			bool isRequired = false;
			std::vector<Choice> choices;
			std::vector<Option> options;

			JSONStructStart
				std::make_tuple(
					json::pair<json::EnumTypeHelper     >(&AppCommand::Option::type       , "type"       , json::REQUIRIED_FIELD),
					json::pair                           (&AppCommand::Option::name       , "name"       , json::REQUIRIED_FIELD),
					json::pair                           (&AppCommand::Option::description, "description", json::OPTIONAL_FIELD ),
					json::pair<json::EnumTypeHelper     >(&AppCommand::Option::isDefault  , "default"    , json::OPTIONAL_FIELD ),
					json::pair                           (&AppCommand::Option::isRequired , "required"   , json::OPTIONAL_FIELD ),
					json::pair<json::ContainerTypeHelper>(&AppCommand::Option::choices    , "choices"    , json::OPTIONAL_FIELD ),
					json::pair<json::ContainerTypeHelper>(&AppCommand::Option::options    , "options"    , json::OPTIONAL_FIELD )
				);
			JSONStructEnd
		};

		struct InteractionData : public DiscordObject {
			InteractionData() = default;
			InteractionData(json::Value & json);
			InteractionData(const nonstd::string_view & json);

			struct Option : public DiscordObject {
				Option() = default;
				Option(json::Value & json);
				Option(const nonstd::string_view & json);

				std::string name;
				json::Value value;
				std::vector<Option> options;

				//warning crashes on type error
				template<class Type>
				Type get() {
					return json::ClassTypeHelper<Type>::toType(value);
				}

				template<class Type>
				bool get(Type& target) {
					return json::castValue<json::ClassTypeHelper<Type>>(target, value);
				}

				JSONStructStart
					std::make_tuple(
						json::pair                           (&AppCommand::InteractionData::Option::name   , "name"          , json::OPTIONAL_FIELD),
						json::pair                           (&AppCommand::InteractionData::Option::value  , "value"         , json::OPTIONAL_FIELD),
						json::pair<json::ContainerTypeHelper>(&AppCommand::InteractionData::Option::options, "options"       , json::OPTIONAL_FIELD)
					);
				JSONStructEnd
			};

			Snowflake<AppCommand> ID;
			std::string name;
			std::vector<Option> options;
			std::string customID;
			ComponentType componentType;

			JSONStructStart
				std::make_tuple(
					json::pair                           (&AppCommand::InteractionData::ID           , "id"            , json::OPTIONAL_FIELD),
					json::pair                           (&AppCommand::InteractionData::name         , "name"          , json::OPTIONAL_FIELD),
					json::pair<json::ContainerTypeHelper>(&AppCommand::InteractionData::options      , "options"       , json::OPTIONAL_FIELD),
					json::pair                           (&AppCommand::InteractionData::customID     , "custom_id"     , json::OPTIONAL_FIELD),
					json::pair<json::EnumTypeHelper     >(&AppCommand::InteractionData::componentType, "component_type", json::OPTIONAL_FIELD)
				);
			JSONStructEnd
		};

		struct Permissions : IdentifiableDiscordObject<DiscordObject> {
			enum class Type : int {
				ROLE = 1,
				USER = 2
			};
			Permissions::Type Type;
			bool Permission;

			JSONStructStart
				std::make_tuple(
					json::pair                      (&Permissions::ID        , "id"        , json::REQUIRIED_FIELD),
					json::pair<json::EnumTypeHelper>(&Permissions::Type      , "type"      , json::REQUIRIED_FIELD),
					json::pair                      (&Permissions::Permission, "permission", json::REQUIRIED_FIELD)
				);
			JSONStructEnd
		};

		Snowflake<DiscordObject> applicationID;
		std::string name;
		std::string description;
		std::vector<Option> options;
		Snowflake<Server> serverID;

		using EmptyOptions = std::array<EmptyDiscordObject, 0>;
		static constexpr EmptyOptions emptyOptions = {};

		JSONStructStart
			std::make_tuple(
				json::pair                           (&AppCommand::ID           , "id"            , json::REQUIRIED_FIELD),
				json::pair                           (&AppCommand::applicationID, "application_id", json::REQUIRIED_FIELD),
				json::pair                           (&AppCommand::name         , "name"          , json::REQUIRIED_FIELD),
				json::pair                           (&AppCommand::description  , "description"   , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&AppCommand::options      , "options"       , json::OPTIONAL_FIELD ),
				json::pair                           (&AppCommand::serverID     , "guild_id"      , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

	struct ServerAppCommandPermissions : IdentifiableDiscordObject<AppCommand> {
		Snowflake<User> applicationID;
		Snowflake<Server> serverID;
		std::vector<AppCommand::Permissions> permissions;

		JSONStructStart
			std::make_tuple(
				json::pair                           (&ServerAppCommandPermissions::ID           , "id"            , json::REQUIRIED_FIELD),
				json::pair                           (&ServerAppCommandPermissions::applicationID, "application_id", json::REQUIRIED_FIELD),
				json::pair                           (&ServerAppCommandPermissions::serverID     , "guild_id"      , json::REQUIRIED_FIELD),
				json::pair<json::ContainerTypeHelper>(&ServerAppCommandPermissions::permissions  , "permissions"   , json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};

	template<>
	struct GetDefault<AppCommand::Option::Default> {
		static inline const AppCommand::Option::Default get() {
			return AppCommand::Option::Default::Undefined;
		}
	};

	template<>
	inline void AppCommand::Option::Choice::set<decltype(nullptr)>(decltype(nullptr)&) {
		value.SetNull();
	}

	template<>
	inline void AppCommand::Option::Choice::set<json::Value>(json::Value& _val) {
		value = _val; //moves
	}

	struct InteractionAppCommandCallbackData : public EditWebhookParams {
		InteractionAppCommandCallbackData() = default;
		InteractionAppCommandCallbackData(json::Value & json);
		InteractionAppCommandCallbackData(const nonstd::string_view & json);

		inline const bool empty() const { return content.empty() && embeds.empty() && components.empty(); }

		bool tts = false;
		enum class Flags : int {
			NONE = 0,
			Ephemeral = 64
		} flags = Flags::NONE;

		JSONStructStart
			std::make_tuple(
				json::pair                           (&InteractionAppCommandCallbackData::tts            , "tts"             , json::OPTIONAL_FIELD),
				json::pair                           (&InteractionAppCommandCallbackData::content        , "content"         , json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&InteractionAppCommandCallbackData::embeds         , "embeds"          , json::OPTIONAL_FIELD),
				json::pair                           (&InteractionAppCommandCallbackData::allowedMentions, "allowed_mentions", json::OPTIONAL_FIELD),
				json::pair<json::EnumTypeHelper     >(&InteractionAppCommandCallbackData::flags          , "flags"           , json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&InteractionAppCommandCallbackData::components     , "components"      , json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	struct Interaction : IdentifiableDiscordObject<Interaction> {
		Interaction() = default;
		Interaction(json::Value & json);
		Interaction(const nonstd::string_view & json);

		enum class CallbackType : int {
			NONE                             = 0, //made up type
			Pong                             = 1,
			Acknowledge                      = 2,
			ChannelMessage                   = 3,
			ChannelMessageWithSource         = 4,
			DeferredChannelMessageWithSource = 5,
			DeferredUpdateMessage            = 6,
			UpdateMessage                    = 7,
		};

		struct Response : public DiscordObject {
			Response() = default;
			Response(json::Value & json);
			Response(const nonstd::string_view & json);

			using Type = CallbackType;
			Type type;
			InteractionAppCommandCallbackData data;

			JSONStructStart
				std::make_tuple(
					json::pair<json::EnumTypeHelper>(&Interaction::Response::type, "type", json::REQUIRIED_FIELD),
					json::pair                      (&Interaction::Response::data, "data", json::OPTIONAL_FIELD )
				);
			JSONStructEnd
		};

		using AppCommandCallbackData = InteractionAppCommandCallbackData;
		using Type = InteractionType;
		InteractionType type;
		Snowflake<DiscordObject> applicationID;
		AppCommand::InteractionData data;
		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		ServerMember member;
		User user;
		std::string token;
		int version = 1;
		Message message;

		JSONStructStart
			std::make_tuple(
				json::pair                      (&Interaction::ID           , "id"            , json::REQUIRIED_FIELD),
				json::pair                      (&Interaction::applicationID, "application_id", json::OPTIONAL_FIELD ),
				json::pair<json::EnumTypeHelper>(&Interaction::type         , "type"          , json::REQUIRIED_FIELD),
				json::pair                      (&Interaction::data         , "data"          , json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::serverID     , "guild_id"      , json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::channelID    , "channel_id"    , json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::member       , "member"        , json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::user         , "user"          , json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::token        , "token"         , json::OPTIONAL_FIELD ),
				json::pair<                   1>(&Interaction::version      , "version"       , json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::message      , "message"       , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

}; // namespace SleepyDiscord