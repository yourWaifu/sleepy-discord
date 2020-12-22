#pragma once
#include <vector>
#include "discord_object_interface.h"
#include "snowflake.h"
#include "server.h"
#include "channel.h"
#include "embed.h"
#include "message.h"

namespace SleepyDiscord {

	struct ApplicationCommand : public IdentifiableDiscordObject<ApplicationCommand> {
		ApplicationCommand() = default;
		ApplicationCommand(json::Value & json);
		ApplicationCommand(const nonstd::string_view & json);
		
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
						json::pair(&ApplicationCommand::Option::Choice::name , "name" , json::REQUIRIED_FIELD),
						json::pair(&ApplicationCommand::Option::Choice::value, "value", json::REQUIRIED_FIELD)
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
					json::pair<json::EnumTypeHelper     >(&ApplicationCommand::Option::type       , "type"       , json::REQUIRIED_FIELD),
					json::pair                           (&ApplicationCommand::Option::name       , "name"       , json::REQUIRIED_FIELD),
					json::pair                           (&ApplicationCommand::Option::description, "description", json::OPTIONAL_FIELD ),
					json::pair<json::EnumTypeHelper     >(&ApplicationCommand::Option::isDefault  , "default"    , json::OPTIONAL_FIELD ),
					json::pair                           (&ApplicationCommand::Option::isRequired , "required"   , json::OPTIONAL_FIELD ),
					json::pair<json::ContainerTypeHelper>(&ApplicationCommand::Option::choices    , "choices"    , json::OPTIONAL_FIELD ),
					json::pair<json::ContainerTypeHelper>(&ApplicationCommand::Option::options    , "options"    , json::OPTIONAL_FIELD )
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
						json::pair                           (&ApplicationCommand::InteractionData::Option::name   , "name"   , json::REQUIRIED_FIELD),
						json::pair                           (&ApplicationCommand::InteractionData::Option::value  , "value"  , json::OPTIONAL_FIELD ),
						json::pair<json::ContainerTypeHelper>(&ApplicationCommand::InteractionData::Option::options, "options", json::OPTIONAL_FIELD )
					);
				JSONStructEnd
			};

			Snowflake<ApplicationCommand> ID;
			std::string name;
			std::vector<Option> options;

			JSONStructStart
				std::make_tuple(
					json::pair                           (&ApplicationCommand::InteractionData::ID     , "id"     , json::REQUIRIED_FIELD),
					json::pair                           (&ApplicationCommand::InteractionData::name   , "name"   , json::REQUIRIED_FIELD),
					json::pair<json::ContainerTypeHelper>(&ApplicationCommand::InteractionData::options, "options", json::OPTIONAL_FIELD )
				);
			JSONStructEnd
		};

		Snowflake<DiscordObject> applicationID;
		std::string name;
		std::string description;
		std::vector<Option> options;

		JSONStructStart
			std::make_tuple(
				json::pair                           (&ApplicationCommand::ID           , "id"            , json::REQUIRIED_FIELD),
				json::pair                           (&ApplicationCommand::applicationID, "application_id", json::REQUIRIED_FIELD),
				json::pair                           (&ApplicationCommand::name         , "name"          , json::REQUIRIED_FIELD),
				json::pair                           (&ApplicationCommand::description  , "description"   , json::OPTIONAL_FIELD ),
				json::pair<json::ContainerTypeHelper>(&ApplicationCommand::options      , "options"       , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

	template<>
	struct GetDefault<ApplicationCommand::Option::Default> {
		static inline const ApplicationCommand::Option::Default get() {
			return ApplicationCommand::Option::Default::Undefined;
		}
	};

	template<>
	inline void ApplicationCommand::Option::Choice::set<decltype(nullptr)>(decltype(nullptr)&) {
		value.SetNull();
	}

	template<>
	inline void ApplicationCommand::Option::Choice::set<json::Value>(json::Value& _val) {
		value = _val; //moves
	}

	struct InteractionApplicationCommandCallbackData : public DiscordObject {
		InteractionApplicationCommandCallbackData() = default;
		InteractionApplicationCommandCallbackData(const json::Value & json);
		InteractionApplicationCommandCallbackData(const nonstd::string_view & json);
		bool tts = false;
		std::string content;
		std::vector<Embed> embeds;
		AllowedMentions allowedMentions;

		JSONStructStart
			std::make_tuple(
				json::pair                           (&InteractionApplicationCommandCallbackData::tts            , "tts"             , json::OPTIONAL_FIELD ),
				json::pair                           (&InteractionApplicationCommandCallbackData::content        , "content"         , json::REQUIRIED_FIELD),
				json::pair<json::ContainerTypeHelper>(&InteractionApplicationCommandCallbackData::embeds         , "embeds"          , json::OPTIONAL_FIELD ),
				json::pair                           (&InteractionApplicationCommandCallbackData::allowedMentions, "allowed_mentions", json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

	struct Interaction : IdentifiableDiscordObject<Interaction> {
		Interaction() = default;
		Interaction(json::Value & json);
		Interaction(const nonstd::string_view & json);

		enum class Type : int {
			NONE = 0, //made up type
			Ping = 1,
			ApplicationCommand = 2,
		};

		struct Response : public DiscordObject {
			Response() = default;
			Response(const json::Value & json);
			Response(const nonstd::string_view & json);

			enum class Type : int {
				NONE = 0, //made up type
				Pong = 1,
				Acknowledge = 2,
				ChannelMessage = 3,
				ChannelMessageWithSource = 4,
				ACKWithSource = 5,
			};

			Type type;
			InteractionApplicationCommandCallbackData data;

			JSONStructStart
				std::make_tuple(
					json::pair<json::EnumTypeHelper>(&Interaction::Response::type, "type", json::REQUIRIED_FIELD),
					json::pair                      (&Interaction::Response::data, "data", json::OPTIONAL_FIELD )
				);
			JSONStructEnd
		};

		Type type;
		ApplicationCommand::InteractionData data;
		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		ServerMember member;
		std::string token;
		int version = 1;

		JSONStructStart
			std::make_tuple(
				json::pair                      (&Interaction::ID       , "id"        , json::REQUIRIED_FIELD),
				json::pair<json::EnumTypeHelper>(&Interaction::type     , "type"      , json::REQUIRIED_FIELD),
				json::pair                      (&Interaction::data     , "data"      , json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::serverID , "guild_id"  , json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::channelID, "channel_id", json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::member   , "member"    , json::OPTIONAL_FIELD ),
				json::pair                      (&Interaction::token    , "token"     , json::OPTIONAL_FIELD ),
				json::pair<                   1>(&Interaction::version  , "version"   , json::OPTIONAL_FIELD )
			);
		JSONStructEnd
	};

}; // namespace SleepyDiscord