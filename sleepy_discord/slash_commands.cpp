#include "slash_commands.h"

namespace SleepyDiscord {
	AppCommand::AppCommand(json::Value & json) :
		AppCommand(json::fromJSON<AppCommand>(json)) {
	}
	AppCommand::AppCommand(const nonstd::string_view & json) :
		AppCommand(json::fromJSON<AppCommand>(json)) {
	}
	AppCommand::Option::Option(json::Value & json) :
		AppCommand::Option(json::fromJSON<AppCommand::Option>(json)) {
	}
	AppCommand::Option::Option(const nonstd::string_view & json) :
		AppCommand::Option(json::fromJSON<AppCommand::Option>(json)) {
	}
	AppCommand::Option::Choice::Choice(json::Value & json) :
		AppCommand::Option::Choice(json::fromJSON<AppCommand::Option::Choice>(json)) {
	}
	AppCommand::Option::Choice::Choice(const nonstd::string_view & json) :
		AppCommand::Option::Choice(json::fromJSON<AppCommand::Option::Choice>(json)) {
	}
	AppCommand::InteractionData::InteractionData(json::Value & json) :
		AppCommand::InteractionData(json::fromJSON<AppCommand::InteractionData>(json)) {
	}
	AppCommand::InteractionData::InteractionData(const nonstd::string_view & json) :
		AppCommand::InteractionData(json::fromJSON<AppCommand::InteractionData>(json)) {
	}
	AppCommand::InteractionData::Option::Option(json::Value & json) :
		AppCommand::InteractionData::Option(json::fromJSON<AppCommand::InteractionData::Option>(json)) {
	}
	AppCommand::InteractionData::Option::Option(const nonstd::string_view & json) :
		AppCommand::InteractionData::Option(json::fromJSON<AppCommand::InteractionData::Option>(json)) {
	}
	InteractionAppCommandCallbackData::InteractionAppCommandCallbackData(json::Value & json) :
		InteractionAppCommandCallbackData(json::fromJSON<InteractionAppCommandCallbackData>(json)) {
	}
	InteractionAppCommandCallbackData::InteractionAppCommandCallbackData(const nonstd::string_view & json) :
		InteractionAppCommandCallbackData(json::fromJSON<InteractionAppCommandCallbackData>(json)) {
	}
	Interaction::Interaction(json::Value & json) :
		Interaction(json::fromJSON<Interaction>(json)) {
	}
	Interaction::Interaction(const nonstd::string_view & json) :
		Interaction(json::fromJSON<Interaction>(json)) {
	}
	Interaction::Response::Response(json::Value & json) :
		Interaction::Response(json::fromJSON<Interaction::Response>(json)) {
	}
	Interaction::Response::Response(const nonstd::string_view & json) :
		Interaction::Response(json::fromJSON<Interaction::Response>(json)) {
	}
}