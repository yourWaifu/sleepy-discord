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
	
	struct InteractionData;

	struct AppCommand : public IdentifiableDiscordObject<AppCommand> {
		AppCommand() = default;
		AppCommand(json::Value & json);
		AppCommand(const nonstd::string_view& json) :
			AppCommand(json::fromJSON<AppCommand>(json)) {}
		
		struct Option : public DiscordObject {
			Option() = default;
			Option(json::Value & json);
			Option(const nonstd::string_view& json) :
				AppCommand::Option(json::fromJSON<AppCommand::Option>(json)) {}

			enum class Type : int {
				NONE              = 0, //made up type
				SUB_COMMAND       = 1,
				SUB_COMMAND_GROUP = 2,
				STRING            = 3,
				INTEGER	          = 4,
				BOOLEAN	          = 5,
				USER              = 6,
				CHANNEL           = 7,
				ROLE              = 8,
				MENTIONABLE       = 9,
				NUMBER            = 10
			};

			template<class T, class C = void>
			struct TypeHelper {};

			template<Type type, class T>
			struct TypeHelperImpl {
				static constexpr Type getType() { return _type; }
			private:
				static constexpr Type _type = type;
			};

			template<> struct TypeHelper<bool, void> : public TypeHelperImpl<Type::BOOLEAN, bool    > {};
			template<> struct TypeHelper<std::string, void> : public TypeHelperImpl<Type::STRING, std::string> {};
			template<> struct TypeHelper<Channel, void> : public TypeHelperImpl<Type::CHANNEL, Channel> {};
			template<> struct TypeHelper<Role, void> : public TypeHelperImpl<Type::ROLE, Role> {};
			template<> struct TypeHelper<User, void> : public TypeHelperImpl<Type::USER, User> {};
			template<class IntType>
			struct TypeHelper<
				IntType, typename std::enable_if<std::is_integral<IntType>::value>::type
			> : public TypeHelperImpl<Type::INTEGER, IntType> {};
			template<class FloatType>
			struct TypeHelper<
				FloatType, typename std::enable_if<std::is_floating_point<FloatType>::value>::type
			> : public TypeHelperImpl<Type::NUMBER, FloatType> {};

			struct Choice : public DiscordObject {
				Choice() = default;
				Choice(json::Value & json);
				Choice(const nonstd::string_view& json) :
					AppCommand::Option::Choice(json::fromJSON<AppCommand::Option::Choice>(json)) {}

				std::string name;
				json::Value value;

				//warning crashes if type isn't correct
				template<class Type>
				inline const Type get() {
					return json::ClassTypeHelper<Type>::toType(value);
				}

				template<class Type>
				inline const bool get(Type& target) {
					return json::castValue<json::ClassTypeHelper<Type>>(target, value);
				}

				template<class Type>
				inline void setCopy(Type& val) {
					value = json::copy(json::ClassTypeHelper<Type>::fromType(val));
				}

				template<class Type>
				inline void setView(Type& val) {
					value = json::ClassTypeHelper<Type>::fromType(val);
				}

				Choice copy() {
					Choice choice;
					choice.name = name;
					choice.value = json::copy(value);
					return choice;
				}

				template<class Type>
				inline void set(Type& val) {
					setView(val);
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
			std::vector<Channel::ChannelType> channelTypes;
			double minValue = std::numeric_limits<double>::quiet_NaN();
			double maxValue = std::numeric_limits<double>::quiet_NaN();
			bool autocomplete = false;

			JSONStructStart
				std::make_tuple(
					json::pair<json::EnumTypeHelper     >(&AppCommand::Option::type        , "type"        , json::REQUIRIED_FIELD),
					json::pair                           (&AppCommand::Option::name        , "name"        , json::REQUIRIED_FIELD),
					json::pair                           (&AppCommand::Option::description , "description" , json::OPTIONAL_FIELD ),
					json::pair                           (&AppCommand::Option::isRequired  , "required"    , json::OPTIONAL_FIELD ),
					json::pair<json::ContainerTypeHelper>(&AppCommand::Option::choices     , "choices"     , json::OPTIONAL_FIELD ),
					json::pair<json::ContainerTypeHelper>(&AppCommand::Option::options     , "options"     , json::OPTIONAL_FIELD ),
					json::pair                           (&AppCommand::Option::autocomplete, "autocomplete", json::OPTIONAL_FIELD )
				);
			JSONStructEnd
		};

		struct Permissions : public IdentifiableDiscordObject<DiscordObject> {
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

		using InteractionData = SleepyDiscord::InteractionData;

		enum class Type : int {
			NONE       = 0, //made up for interal library use
			CHAT_INPUT = 1,
			USER       = 2,
			MESSAGE    = 3
		};
		Type type = Type::CHAT_INPUT;
		Snowflake<DiscordObject> applicationID;
		std::string name;
		std::string description;
		std::vector<Option> options;
		Snowflake<Server> serverID;
		bool defaultPermission = true;
		Snowflake<DiscordObject> version;

		using EmptyOptions = std::array<EmptyDiscordObject, 0>;
		static constexpr EmptyOptions emptyOptions = EmptyOptions{};

		JSONStructStart
			std::make_tuple(
				json::pair                           (&AppCommand::ID               , "id"                , json::REQUIRIED_FIELD),
				json::pair<json::EnumTypeHelper     >(&AppCommand::type             , "type"              , json::OPTIONAL_FIELD ),
				json::pair                           (&AppCommand::applicationID    , "application_id"    , json::REQUIRIED_FIELD),
				json::pair                           (&AppCommand::name             , "name"              , json::REQUIRIED_FIELD),
				json::pair                           (&AppCommand::description      , "description"       , json::OPTIONAL_FIELD ),
				json::pair                           (&AppCommand::defaultPermission, "default_permission", json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&AppCommand::options          , "options"           , json::OPTIONAL_FIELD ),
				json::pair                           (&AppCommand::serverID         , "guild_id"          , json::OPTIONAL_FIELD ),
				json::pair                           (&AppCommand::version          , "version"           , json::OPTIONAL_FIELD )
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
		InteractionAppCommandCallbackData(const nonstd::string_view& json) :
			InteractionAppCommandCallbackData(json::fromJSON<InteractionAppCommandCallbackData>(json)) {}

		inline const bool empty() const { return content.empty() && embeds.empty() && components.empty(); }

		bool tts = false;
		enum class Flags : int {
			NONE = 0,
			Ephemeral = 1 << 6
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

	struct InteractionData : public DiscordObject {
		InteractionData() = default;
		InteractionData(json::Value& json);
		InteractionData(const nonstd::string_view& json) :
			InteractionData(json::fromJSON<InteractionData>(json)) {}

		struct Option : public DiscordObject {
			Option() = default;
			Option(json::Value& json);
			Option(const nonstd::string_view& json) :
				InteractionData::Option(json::fromJSON<InteractionData::Option>(json)) {}

			std::string name;
			AppCommand::Option::Type type = AppCommand::Option::Type::NONE;
			json::Value value;
			std::vector<Option> options;
			bool focused = false;

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
					json::pair                           (&InteractionData::Option::name   , "name"   , json::OPTIONAL_FIELD),
					json::pair<json::EnumTypeHelper     >(&InteractionData::Option::type   , "type"   , json::OPTIONAL_FIELD),
					json::pair                           (&InteractionData::Option::value  , "value"  , json::OPTIONAL_FIELD),
					json::pair<json::ContainerTypeHelper>(&InteractionData::Option::options, "options", json::OPTIONAL_FIELD),
					json::pair                           (&InteractionData::Option::focused, "focused", json::OPTIONAL_FIELD)
				);
			JSONStructEnd
		};

		Snowflake<AppCommand> ID;
		std::string name;
		AppCommand::Type type = AppCommand::Type::NONE;
		std::vector<Option> options;
		std::string customID;
		ComponentType componentType;
		std::vector<std::string> values;
		Snowflake<DiscordObject> targetID;

		JSONStructStart
			std::make_tuple(
				json::pair                           (&InteractionData::ID           , "id"            , json::OPTIONAL_FIELD),
				json::pair                           (&InteractionData::name         , "name"          , json::OPTIONAL_FIELD),
				json::pair<json::EnumTypeHelper     >(&InteractionData::type         , "type"          , json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&InteractionData::options      , "options"       , json::OPTIONAL_FIELD),
				json::pair                           (&InteractionData::customID     , "custom_id"     , json::OPTIONAL_FIELD),
				json::pair<json::EnumTypeHelper     >(&InteractionData::componentType, "component_type", json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&InteractionData::values       , "values"        , json::OPTIONAL_FIELD),
				json::pair                           (&InteractionData::targetID     , "target_id"     , json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	/*
	* to do implement json maps first
	struct ResolvedData : public DiscordObject {
		ResolvedData() = default;
		ResolvedData(json::Value& json);
		ResolvedData(const nonstd::string_view& json) :
			ResolvedData(json::fromJSON<ResolvedData>(json)) {}


	};
	*/

	struct Interaction : IdentifiableDiscordObject<Interaction> {
		Interaction() = default;
		Interaction(json::Value & json);
		Interaction(const nonstd::string_view& json) :
			Interaction(json::fromJSON<Interaction>(json)) {}

		enum class CallbackType : int {
			NONE                                 = 0, //made up type
			Pong                                 = 1,
			ChannelMessageWithSource             = 4,
			DeferredChannelMessageWithSource     = 5,
			DeferredUpdateMessage                = 6,
			UpdateMessage                        = 7,
			ApplicationCommandAutocompleteResult = 8,
		};

		struct Response : public DiscordObject {
			Response() = default;
			Response(json::Value & json);
			Response(const nonstd::string_view& json) :
				Interaction::Response(json::fromJSON<Interaction::Response>(json)) {}

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
		InteractionData data;
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

	struct Autocomplete : public DiscordObject {
		Autocomplete() = default;
		Autocomplete(json::Value& json);
		Autocomplete(const nonstd::string_view& json) :
			Autocomplete(json::fromJSON<Autocomplete>(json)) {}

		std::vector<AppCommand::Option::Choice> choices;

		JSONStructStart
			std::make_tuple(
				json::pair<json::ContainerTypeHelper>(&Autocomplete::choices, "choices", json::REQUIRIED_FIELD)
			);
		JSONStructEnd
	};

}; // namespace SleepyDiscord