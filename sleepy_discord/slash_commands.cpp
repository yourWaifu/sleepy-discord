#include "slash_commands.h"

namespace SleepyDiscord {
	ApplicationCommand::ApplicationCommand(json::Value & json) :
		ApplicationCommand(json::fromJSON<ApplicationCommand>(json)) {
	}
	ApplicationCommand::ApplicationCommand(const nonstd::string_view & json) :
		ApplicationCommand(json::fromJSON<ApplicationCommand>(json)) {
	}
	ApplicationCommand::Option::Option(json::Value & json) :
		ApplicationCommand::Option(json::fromJSON<ApplicationCommand::Option>(json)) {
	}
	ApplicationCommand::Option::Option(const nonstd::string_view & json) :
		ApplicationCommand::Option(json::fromJSON<ApplicationCommand::Option>(json)) {
	}
	ApplicationCommand::Option::Choice::Choice(json::Value & json) :
		ApplicationCommand::Option::Choice(json::fromJSON<ApplicationCommand::Option::Choice>(json)) {
	}
	ApplicationCommand::Option::Choice::Choice(const nonstd::string_view & json) :
		ApplicationCommand::Option::Choice(json::fromJSON<ApplicationCommand::Option::Choice>(json)) {
	}
	ApplicationCommand::InteractionData::InteractionData(json::Value & json) :
		ApplicationCommand::InteractionData(json::fromJSON<ApplicationCommand::InteractionData>(json)) {
	}
	ApplicationCommand::InteractionData::InteractionData(const nonstd::string_view & json) :
		ApplicationCommand::InteractionData(json::fromJSON<ApplicationCommand::InteractionData>(json)) {
	}
	ApplicationCommand::InteractionData::Option::Option(json::Value & json) :
		ApplicationCommand::InteractionData::Option(json::fromJSON<ApplicationCommand::InteractionData::Option>(json)) {
	}
	ApplicationCommand::InteractionData::Option::Option(const nonstd::string_view & json) :
		ApplicationCommand::InteractionData::Option(json::fromJSON<ApplicationCommand::InteractionData::Option>(json)) {
	}
	InteractionApplicationCommandCallbackData::InteractionApplicationCommandCallbackData(const json::Value & json) :
		InteractionApplicationCommandCallbackData(json::fromJSON<InteractionApplicationCommandCallbackData>(json)) {
	}
	InteractionApplicationCommandCallbackData::InteractionApplicationCommandCallbackData(const nonstd::string_view & json) :
		InteractionApplicationCommandCallbackData(json::fromJSON<InteractionApplicationCommandCallbackData>(json)) {
	}
	Interaction::Interaction(json::Value & json) :
		Interaction(json::fromJSON<Interaction>(json)) {
	}
	Interaction::Interaction(const nonstd::string_view & json) :
		Interaction(json::fromJSON<Interaction>(json)) {
	}
	Interaction::Response::Response(const json::Value & json) :
		Interaction::Response(json::fromJSON<Interaction::Response>(json)) {
	}
	Interaction::Response::Response(const nonstd::string_view & json) :
		Interaction::Response(json::fromJSON<Interaction::Response>(json)) {
	}
}