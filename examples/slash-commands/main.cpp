#include "sleepy_discord/sleepy_discord.h"

//Discord client code
class Client;

namespace Command {
	using Verb = std::function<
		void(
			Client&,
			SleepyDiscord::Interaction&
			)
	>;
	struct Command {
		std::string name;
		Verb verb;
	};
	using MappedCommands = std::unordered_map<std::string, Command>;
	using MappedCommand = MappedCommands::value_type;
	static MappedCommands all;
	static void addCommand(Command command) {
		all.emplace(command.name, command);
	}
}

class Client : public SleepyDiscord::DiscordClient
{
public:
	using SleepyDiscord::DiscordClient::DiscordClient;

	void onReady(SleepyDiscord::Ready ready) override {
		static int connectCount = 0;
		connectCount += 1;
		if (connectCount == 1) {
			onFirstConnect();
		}
	}

	void onFirstConnect() {
		{
			Command::Command test{
				"test", [](
					Client& client,
					SleepyDiscord::Interaction& interaction
				) {
					SleepyDiscord::Interaction::Response response;
					response.type = SleepyDiscord::Interaction::Response::Type::ChannelMessageWithSource;
					response.data.content = "Hello!";
					client.createInteractionResponse(interaction.ID, interaction.token, response);
					return;
				}
			};
			createGlobalAppCommand(getID(), test.name, "tests commands");
			Command::addCommand(test);
		}

		{
			Command::Command add{
				"add", [](
					Client& client,
					SleepyDiscord::Interaction& interaction
				) {
					double answer = 0;
					for (auto& option : interaction.data.options) {
						double num;
						if (option.get(num)) {
							answer += num;
						}
					}
					SleepyDiscord::Interaction::Response response;
					response.type = SleepyDiscord::Interaction::Response::Type::ChannelMessageWithSource;
					response.data.content = std::to_string(answer);
					client.createInteractionResponse(interaction.ID, interaction.token, response);
					return;
				}
			};

			std::vector<SleepyDiscord::AppCommand::Option> options;

			SleepyDiscord::AppCommand::Option left;
			left.name = "left";
			left.isRequired = true;
			left.description = "The number to the left of the + sign";
			left.type = SleepyDiscord::AppCommand::Option::TypeHelper<double>::getType();
			options.push_back(std::move(left));

			SleepyDiscord::AppCommand::Option right;
			right.name = "right";
			right.isRequired = true;
			right.description = "The number to the right of the + sign";
			right.type = SleepyDiscord::AppCommand::Option::TypeHelper<double>::getType();
			options.push_back(std::move(right));

			createGlobalAppCommand(getID(), add.name, "sum of two number", std::move(options));
			Command::addCommand(add);
		}
	}

	void onInteraction(SleepyDiscord::Interaction interaction) override {
		auto foundCommand = Command::all.find(interaction.data.name);
		if (foundCommand == Command::all.end()) {
			//not found
			SleepyDiscord::Interaction::Response response;
			response.type = SleepyDiscord::Interaction::Response::Type::ChannelMessageWithSource;
			response.data.content = "Couldn't find command";
			response.data.flags = SleepyDiscord::InteractionAppCommandCallbackData::Flags::Ephemeral;
			createInteractionResponse(interaction, interaction.token, response);
			return;
		}
		foundCommand->second.verb(*this, interaction);
	}
private:
};

int main()
{
	Client client("NDczMzM5MjY2NTIxNjI4Njcz.W16Niw.I83IsAg4rWUVC8546iyqPS0dvVU", SleepyDiscord::USER_CONTROLED_THREADS);
	client.setIntents(0);
	client.run();

	return 0;
}