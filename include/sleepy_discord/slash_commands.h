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

	// isInt is mostly for the Auto Docs, because without it, auto docs will generate
	// very confusing links that confues the docs framework being used.
	template<class Int>
	using isInt = typename std::enable_if < std::is_integral< Int >::value >;

	template<class Num>
	using isNum = typename std::enable_if < std::is_floating_point< Num >::value >;

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

				//preforms a deep copy
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
			enum class Default : int8_t {
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

		using EmptyOptions = std::nullptr_t;
		static constexpr EmptyOptions emptyOptions = nullptr;

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

	template<> struct AppCommand::Option::TypeHelper<bool       , void> :
		public AppCommand::Option::TypeHelperImpl<Type::BOOLEAN, bool       > {};
	template<> struct AppCommand::Option::TypeHelper<std::string, void> :
		public AppCommand::Option::TypeHelperImpl<Type::STRING , std::string> {};
	template<> struct AppCommand::Option::TypeHelper<const char*, void> :
		public AppCommand::Option::TypeHelperImpl<Type::STRING , std::string> {};
	template<> struct AppCommand::Option::TypeHelper<Channel    , void> :
		public AppCommand::Option::TypeHelperImpl<Type::CHANNEL, Channel    > {};
	template<> struct AppCommand::Option::TypeHelper<Role       , void> :
		public AppCommand::Option::TypeHelperImpl<Type::ROLE   , Role       > {};
	template<> struct AppCommand::Option::TypeHelper<User       , void> :
		public AppCommand::Option::TypeHelperImpl<Type::USER   , User       > {};

	template<class Int>
	struct AppCommand::Option::TypeHelper<
		Int, typename isInt<Int>::type
	> : public AppCommand::Option::TypeHelperImpl< Type::INTEGER, Int > {};

	template<class Num>
	struct AppCommand::Option::TypeHelper<
		Num, typename isNum<Num>::type
	> : public AppCommand::Option::TypeHelperImpl< Type::NUMBER, Num > {};

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

	namespace InteractionCallback {
		struct BaseData : public DiscordObject {};
		struct EmptyData : public DiscordObject {
			EmptyData() = default;
			EmptyData(json::Value & json);
			EmptyData(const nonstd::string_view & json) :
				EmptyData(json::fromJSON<EmptyData>(json)) {}

			inline const bool empty() const {
				return true;
			}

			JSONStructStart
				std::make_tuple();
			JSONStructEnd
		};

		struct Message : public WebHookParams {
			Message() = default;
			Message(json::Value & json);
			Message(const nonstd::string_view& json) :
				Message(json::fromJSON<Message>(json)) {}

			inline const bool empty() const { return content.empty() && embeds.empty() && components.empty(); }

			bool tts = false;
			enum class Flags : int {
				UNDEFINED = -1,
				NONE = 0,
				Ephemeral = 1 << 6
			} flags = Flags::UNDEFINED;

			JSONStructStart
				std::make_tuple(
					json::pair                           (&Message::tts            , "tts"             , json::OPTIONAL_FIELD),
					json::pair                           (&Message::content        , "content"         , json::OPTIONAL_FIELD),
					json::pair<json::ContainerTypeHelper>(&Message::embeds         , "embeds"          , json::OPTIONAL_FIELD),
					json::pair                           (&Message::allowedMentions, "allowed_mentions", json::OPTIONAL_FIELD),
					json::pair<json::EnumTypeHelper     >(&Message::flags          , "flags"           , json::OPTIONAL_FIELD),
					json::pair<json::ContainerTypeHelper>(&Message::components     , "components"      , json::OPTIONAL_FIELD)
				);
			JSONStructEnd
		};

		struct EditMessage : public BaseEditWebhookParams<EditWebhookParams> {
			EditMessage() = default;
			EditMessage(json::Value& json);
			EditMessage(const nonstd::string_view& json) :
				EditMessage(json::fromJSON<EditMessage>(json)) {}

			inline const bool empty() const { return !(content || embeds || components); }

			Message::Flags flags = Message::Flags::UNDEFINED;

			JSONStructStart
				std::tuple_cat(
					BaseEditWebhookParams<EditWebhookParams>::JSONStruct,
					std::make_tuple(
						json::pair<json::EnumTypeHelper>(&EditMessage::flags, "flags", json::OPTIONAL_FIELD)
					)
				);
			JSONStructEnd
		};

		struct Autocomplete : public DiscordObject {
			Autocomplete() = default;
			Autocomplete(json::Value & json);
			Autocomplete(const nonstd::string_view & json) :
				Autocomplete(json::fromJSON<Autocomplete>(json)) {}

			std::vector<AppCommand::Option::Choice> choices;

			bool empty() const {
				return choices.empty();
			}

			JSONStructStart
				std::make_tuple(
					json::pair<json::ContainerTypeHelper>(&Autocomplete::choices, "choices", json::REQUIRIED_FIELD)
				);
			JSONStructEnd
		};

		struct Modal : public DiscordObject {
			Modal() = default;
			Modal(json::Value & json);
			Modal(const nonstd::string_view& json) :
				Modal(json::fromJSON<Modal>(json)) {}

			inline const bool empty() const { return components.empty(); }

			std::string customID;
			std::string title;
			std::vector<std::shared_ptr<BaseComponent>> components;

			JSONStructStart
				std::make_tuple(
					json::pair                           (&Modal::customID  , "custom_id" , json::REQUIRIED_FIELD),
					json::pair                           (&Modal::title     , "title"     , json::REQUIRIED_FIELD),
					json::pair<json::ContainerTypeHelper>(&Modal::components, "components", json::REQUIRIED_FIELD)
				);
			JSONStructEnd
		};
	}

	template<>
	struct GetDefault<InteractionCallback::Message::Flags> {
		static inline const InteractionCallback::Message::Flags get() {
			return InteractionCallback::Message::Flags::UNDEFINED;
		}
	};

	// for backwards compatibility, oops
	using InteractionAppCommandCallbackData = InteractionCallback::Message;

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

		struct ResolvedData : public DiscordObject {
			ResolvedData() = default;
			ResolvedData(json::Value& json);
			ResolvedData(const nonstd::string_view& json) :
				ResolvedData(json::fromJSON<ResolvedData>(json)) {}

			template<typename Type>
			struct Data {
				using JSONTypeHelper = json::ClassTypeHelper<json::Value>;
				using Identifier = typename Type::Identifier;

				Data() = default;
				Data(json::Value & json) {
					data = JSONTypeHelper::toType(json); //moves
				}
				Data(const nonstd::string_view & json) :
					Data(json::fromJSON<Data>(json)) {}

			private:

				inline json::Value::MemberIterator find(const std::string& ID) {
					return data.FindMember(
						json::Value(json::Value::StringRefType(ID.c_str(), ID.length()))
					);
				}

				const bool find(const std::string& ID, Type& object) {
					auto member = find(ID);
					if (member != data.MemberEnd()) {
						object = member->value;
						return true;
					}
					return false;
				}

			public:

				inline json::Value serialize(typename json::Value::AllocatorType& alloc) const {
					return JSONTypeHelper::fromType(data, alloc);
				}

				inline const bool empty() const { return JSONTypeHelper::empty(data); }

				static inline const bool isType(const typename json::Value& value) {
					return value.IsObject();
				}

				inline json::Value::MemberIterator find(const Identifier& ID) {
					return find(ID.string());
				}
				
				// useful for target_id since it can be a snowflake muliple discord objects
				inline json::Value::MemberIterator find(const Snowflake<DiscordObject>& ID) {
					return find(ID.string());
				}

				inline json::Value::MemberIterator end() {
					return data.MemberEnd();
				}

				inline Type cast(json::Value::MemberIterator& member) {
					return member->value;
				}

				inline const bool find(const Identifier& ID, Type& object) {
					return find(ID.string(), object);
				}

				inline const bool find(const Snowflake<DiscordObject>& ID, Type& object) {
					return find(ID.string(), object);
				}

				inline std::unordered_map<Identifier, Type> createUnorderedMap() {
					return json::MapTypeHelper<
						std::unordered_map<Identifier, Type>,
						json::ClassTypeHelper>::toType(data);
				}

				json::Value data;
			};

			Data<User> users;
			Data<ServerMember> members;
			Data<Role> roles;
			Data<Channel> channels;
			Data<Message> messages;
			Data<Attachment> attachment;

			//Note: JSON structure works very different here
			//JSON is used as a dictionary here
			JSONStructStart
				std::make_tuple(
					json::pair(&InteractionData::ResolvedData::users     , "users"      , json::OPTIONAL_FIELD),
					json::pair(&InteractionData::ResolvedData::members   , "members"    , json::OPTIONAL_FIELD),
					json::pair(&InteractionData::ResolvedData::roles     , "roles"      , json::OPTIONAL_FIELD),
					json::pair(&InteractionData::ResolvedData::channels  , "channels"   , json::OPTIONAL_FIELD),
					json::pair(&InteractionData::ResolvedData::messages  , "messages"   , json::OPTIONAL_FIELD),
					json::pair(&InteractionData::ResolvedData::attachment, "attachments", json::OPTIONAL_FIELD)
				);
			JSONStructEnd
		};

		Snowflake<AppCommand> ID;
		std::string name;
		AppCommand::Type type = AppCommand::Type::NONE;
		ResolvedData resolved;
		std::vector<Option> options;
		std::string customID;
		ComponentType componentType;
		std::vector<std::string> values;
		Snowflake<DiscordObject> targetID;
		std::vector<std::shared_ptr<BaseComponent>> components;

		JSONStructStart
			std::make_tuple(
				json::pair                           (&InteractionData::ID           , "id"            , json::OPTIONAL_FIELD),
				json::pair                           (&InteractionData::name         , "name"          , json::OPTIONAL_FIELD),
				json::pair<json::EnumTypeHelper     >(&InteractionData::type         , "type"          , json::OPTIONAL_FIELD),
				json::pair                           (&InteractionData::resolved     , "resolved"      , json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&InteractionData::options      , "options"       , json::OPTIONAL_FIELD),
				json::pair                           (&InteractionData::customID     , "custom_id"     , json::OPTIONAL_FIELD),
				json::pair<json::EnumTypeHelper     >(&InteractionData::componentType, "component_type", json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&InteractionData::values       , "values"        , json::OPTIONAL_FIELD),
				json::pair                           (&InteractionData::targetID     , "target_id"     , json::OPTIONAL_FIELD),
				json::pair<json::ContainerTypeHelper>(&InteractionData::components   , "components"    , json::OPTIONAL_FIELD)
			);
		JSONStructEnd
	};

	// The names of these Types and Enum values are so long that it's
	// causing doxybook2 to output links that are cut off.
	// I'm aware that the short hands make it harder to read.

	enum class IntCallBackT : int {
		NONE = 0, //made up type
		Pong = 1,
		ChannelMessageWithSource = 4,
		DeferredChannelMessageWithSource = 5,
		DefChannelMessageWScore = DeferredChannelMessageWithSource, //Def = deferred W = with
		DeferredUpdateMessage = 6,
		UpdateMessage = 7,
		ApplicationCommandAutocompleteResult = 8,
		AppCommandAutocomplete = ApplicationCommandAutocompleteResult, //short hand
		Modal = 9
	};

	//shorthand
	using InteractionCallbackType = IntCallBackT;

	template<InteractionCallbackType type>
	struct InteractionCallbackTypeHelper {
		using Type = json::Value;
	};
	template<> struct InteractionCallbackTypeHelper<IntCallBackT::Pong> {
		using Type = InteractionCallback::EmptyData;
	};
	template<> struct InteractionCallbackTypeHelper<IntCallBackT::ChannelMessageWithSource> {
		using Type = InteractionCallback::Message;
	};
	template<> struct InteractionCallbackTypeHelper<IntCallBackT::DefChannelMessageWScore> {
		using Type = InteractionCallback::EmptyData;
	};
	template<> struct InteractionCallbackTypeHelper<IntCallBackT::DeferredUpdateMessage> {
		using Type = InteractionCallback::EmptyData;
	};
	template<> struct InteractionCallbackTypeHelper<IntCallBackT::UpdateMessage> {
		using Type = InteractionCallback::EditMessage;
	};
	template<> struct InteractionCallbackTypeHelper<IntCallBackT::AppCommandAutocomplete> {
		using Type = InteractionCallback::Autocomplete;
	};
	template<> struct InteractionCallbackTypeHelper<IntCallBackT::Modal> {
		using Type = InteractionCallback::Modal;
	};
	template<typename Type>
	struct InteractionCallbackHelper {
		static constexpr InteractionCallbackType getType() { return InteractionCallbackType::NONE; }
	};
	template<> struct InteractionCallbackHelper<InteractionCallback::Autocomplete> {
		static constexpr InteractionCallbackType getType() { return InteractionCallbackType::ApplicationCommandAutocompleteResult; }
	};
	template<> struct InteractionCallbackHelper<InteractionCallback::EditMessage> {
		static constexpr InteractionCallbackType getType() { return InteractionCallbackType::UpdateMessage; }
	};
	template<> struct InteractionCallbackHelper<InteractionCallback::Modal> {
		static constexpr InteractionCallbackType getType() { return InteractionCallbackType::Modal; }
	};

	struct Interaction : IdentifiableDiscordObject<Interaction> {
		Interaction() = default;
		Interaction(json::Value & json);
		Interaction(const nonstd::string_view& json) :
			Interaction(json::fromJSON<Interaction>(json)) {}

		using CallbackType = InteractionCallbackType;

		template<InteractionCallbackType _type = InteractionCallbackType::ChannelMessageWithSource>
		struct Response : public DiscordObject {
			Response() = default;
			Response(json::Value & json) :
				Response(json::fromJSON<Response>(json)) {}
			Response(const nonstd::string_view& json) :
				Response(json::fromJSON<Response>(json)) {}

			using Type = CallbackType;
			using DataType = typename InteractionCallbackTypeHelper<_type>::Type;
			Type type = _type;
			DataType data;

			JSONStructStart
				std::make_tuple(
					json::pair<json::EnumTypeHelper>(&Interaction::Response<_type>::type, "type", json::REQUIRIED_FIELD),
					json::pair                      (&Interaction::Response<_type>::data, "data", json::OPTIONAL_FIELD)
				);
			JSONStructEnd
		};

		template<typename Type>
		using Callback = Response<InteractionCallbackHelper<Type>::getType()>;

		using AutocompleteResponse = Callback<InteractionCallback::Autocomplete>;
		using MessageResponse = Response<InteractionCallbackType::ChannelMessageWithSource>;
		using EditMessageResponse = Response<InteractionCallbackType::UpdateMessage>;
		using ModalResponse = Response<InteractionCallbackType::Modal>;

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

	struct FollowupMessage : public BaseWebHookParams<FollowupMessage> {
		FollowupMessage() = default;
		FollowupMessage(json::Value& json);
		FollowupMessage(const nonstd::string_view& json) :
			FollowupMessage(json::fromJSON<FollowupMessage>(json)) {}
		InteractionCallback::Message::Flags flags = InteractionCallback::Message::Flags::UNDEFINED;

		JSONStructStart
			std::tuple_cat(
				BaseWebHookParams<FollowupMessage>::JSONStruct,
				std::make_tuple(
					json::pair<json::EnumTypeHelper>(&FollowupMessage::flags, "flags", json::OPTIONAL_FIELD)
				)
			);
		JSONStructEnd
	};

}; // namespace SleepyDiscord
