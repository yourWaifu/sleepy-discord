#include "slash_commands.h"

namespace SleepyDiscord {
	AppCommand::AppCommand(json::Value & json) :
		AppCommand(json::fromJSON<AppCommand>(json)) {
	}
	AppCommand::Option::Option(json::Value & json) :
		AppCommand::Option(json::fromJSON<AppCommand::Option>(json)) {
	}
	AppCommand::Option::Choice::Choice(json::Value & json) :
		AppCommand::Option::Choice(json::fromJSON<AppCommand::Option::Choice>(json)) {
	}
	InteractionData::InteractionData(json::Value & json) :
		InteractionData(json::fromJSON<InteractionData>(json)) {
	}
	InteractionData::Option::Option(json::Value & json) :
		InteractionData::Option(json::fromJSON<InteractionData::Option>(json)) {
	}
	InteractionAppCommandCallbackData::InteractionAppCommandCallbackData(json::Value & json) :
		InteractionAppCommandCallbackData(json::fromJSON<InteractionAppCommandCallbackData>(json)) {
	}
	Interaction::Interaction(json::Value & json) :
		Interaction(json::fromJSON<Interaction>(json)) {
	}
	Interaction::Response::Response(json::Value & json) :
		Interaction::Response(json::fromJSON<Interaction::Response>(json)) {
	}
}