#include <random> //For noise
#include "sleepy_discord/sleepy_discord.h"
#include "IO_file.h" //For music

struct SquareWave : public SleepyDiscord::AudioVectorSource
{
	using SleepyDiscord::AudioVectorSource::AudioVectorSource;
	void read(
		SleepyDiscord::AudioTransmissionDetails &details,
		SleepyDiscord::AudioVectorSource::Container &target) override
	{
		for (SleepyDiscord::AudioSample &sample : target)
		{
			sample = (++sampleOffset / 100) % 2 ? volume : -1 * volume;
		}
	}
	std::size_t sampleOffset = 0;
	int volume = 2000;
	int halfSquareWaveLength = 100;
};

struct Noise : public SleepyDiscord::AudioVectorSource
{
	Noise() : SleepyDiscord::AudioVectorSource(),
			  engine(randomDevice()),
			  distribution(-1.0, 1.0)
	{
	}
	void read(
		SleepyDiscord::AudioTransmissionDetails &details,
		SleepyDiscord::AudioVectorSource::Container &target) override
	{
		for (SleepyDiscord::AudioSample &sample : target)
		{
			sample = static_cast<SleepyDiscord::AudioSample>(
				distribution(engine) * volume);
		}
	}
	std::random_device randomDevice;
	std::mt19937 engine;
	std::uniform_real_distribution<> distribution;
	double volume = 2000.0;
};

struct RawPCMAudioFile : public SleepyDiscord::AudioPointerSource
{
	RawPCMAudioFile(std::string fileName)
	{
		//open file
		File musicFile(fileName);
		const std::size_t fileSize = musicFile.getSize();
		if (fileSize == static_cast<std::size_t>(-1))
			return;

		//read file
		music.reserve(fileSize / sizeof(SleepyDiscord::AudioSample));
		musicFile.get<SleepyDiscord::AudioSample>(music);
		musicLength = music.size();
		progress = 0;
	}
	void read(
		SleepyDiscord::AudioTransmissionDetails &details,
		SleepyDiscord::AudioSample *&buffer, std::size_t &length) override
	{
		constexpr std::size_t proposedLength = SleepyDiscord::AudioTransmissionDetails::proposedLength();
		buffer = &music[progress];
		//if song isn't over, read proposedLength amount of samples
		//else use a length of 0 to stop listening
		length = proposedLength < (musicLength - progress) ? proposedLength : 0;
		progress += proposedLength;
	}
	std::size_t progress = 0;
	std::vector<SleepyDiscord::AudioSample> music;
	std::size_t musicLength = 0;
};

template<class _OnReadyCallback>
class VoiceEvents : public SleepyDiscord::BaseVoiceEventHandler
{
public:
	using OnReadyCallback = _OnReadyCallback;
	VoiceEvents(OnReadyCallback& theOnReadyCallback) 
		: onReadyCallback(theOnReadyCallback) {}

	void onReady(SleepyDiscord::VoiceConnection &connection) override {
		onReadyCallback(connection);
	}

	void onEndSpeaking(SleepyDiscord::VoiceConnection &connection) override {
		connection.disconnect();
	}

private:
	OnReadyCallback onReadyCallback;
};

//bolderplate code
bool startsWith(const std::string& target, const std::string& test) {
	return target.compare(0, test.size(), test) == 0;
}

static inline void trim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

std::queue<std::string> split(const std::string& source) {
	std::stringstream ss(source);
	std::string item;
	std::queue<std::string> target;
	while (std::getline(ss, item, ' '))
		if (!item.empty())
			target.push(item);
	return target;
}

//Discord client code
class SoundPlayerClient;

namespace Command {
	using Verb = std::function<
		void(
			SoundPlayerClient&,
			SleepyDiscord::Message&,
			std::queue<std::string>&
		)
	>;
	struct Command {
		std::string name;
		std::vector<std::string> params;
		Verb verb;
	};
	using MappedCommands = std::unordered_map<std::string, Command>;
	using MappedCommand = MappedCommands::value_type;
	static MappedCommands all;
	static void addCommand(Command command) {
		all.emplace(command.name, command);
	}
}

class SoundPlayerClient : public SleepyDiscord::DiscordClient
{
public:
	using SleepyDiscord::DiscordClient::DiscordClient;

	void onMessage(SleepyDiscord::Message message) override
	{
		if (message.isMentioned(getID()))
		{
			std::queue<std::string> parameters = split(message.content);
			const std::string mention = "<@" + getID().string() + ">";
			const std::string mentionNick = "<@!" + getID().string() + ">";
			if (
				//only allow if has more then 1 parameter 
				parameters.size() <= 1 &&
				//only allow if starts with a mention
				(parameters.front() != mention || parameters.front() != mentionNick)
			)
				return;

			//remove the parameters as we go
			parameters.pop();
			if (parameters.empty())
				return;

			//get command
			Command::MappedCommands::iterator foundCommand =
				Command::all.find(parameters.front());
			if (foundCommand == Command::all.end()) {
				sendMessage(message.channelID, "Error: Command not found", SleepyDiscord::Async);
				return;
			}
			parameters.pop();
			if (
				parameters.size() <
					foundCommand->second.params.size()
			) {
				sendMessage(message.channelID, "Error: Too few parameters", SleepyDiscord::Async);
				return;
			}

			//call command
			foundCommand->second.verb(*this, message, parameters);
		}
	}

	template<class Callback>
	void connectVoice(
		SleepyDiscord::Message& message,
		std::string& channelParm,
		Callback callback
	) {
		const std::string mentionChannelStart = "<#";
		const std::string mentionChannelEnd = ">";
		if (
			!startsWith(channelParm, mentionChannelStart)
		)
			return;
		std::string::size_type channelMentionEnd =
			channelParm.find('>');
		if (channelMentionEnd == std::string::npos)
			return;
		auto channelID = SleepyDiscord::Snowflake<SleepyDiscord::Channel>(
			channelParm.substr(
				mentionChannelStart.size(),
				channelMentionEnd - mentionChannelStart.size()
			)
		);
		SleepyDiscord::VoiceContext& context =
			connectToVoiceChannel(message.serverID, channelID);
		context.startVoiceHandler<VoiceEvents<Callback>>(callback);
	}
private:
};

template<class VoiceSource, class... Types>
Command::Verb createSimpleCommandVerbForVoiceSource(Types&&... arguments) {
	return [&arguments...](
		SoundPlayerClient& client,
		SleepyDiscord::Message& message,
		std::queue<std::string>& parmas
	) {
		std::string& channelParma = parmas.front();
		client.connectVoice(message, channelParma,
			[arguments...](SleepyDiscord::VoiceConnection& connection) {
				connection.startSpeaking<VoiceSource>(
					//audioDataParms to pass over to SquareWave
					arguments...
				);
			}
		);
	};
}

int main()
{
	std::string token;
	{
		File tokenFile("DiscordToken.txt");
		const std::size_t tokenSize = tokenFile.getSize();
		if (tokenSize == static_cast<std::size_t>(-1)) {
			std::cout << "Error: Can't find DiscordToken.txt\n";
			return 1;
		}
		token.resize(tokenSize);
		tokenFile.get<std::string::value_type>(&token[0]);
		trim(token);
	}

	Command::addCommand({
		"help", {}, [](
			SoundPlayerClient& client,
			SleepyDiscord::Message& message,
			std::queue<std::string>&
		) {
			constexpr char start[] = "Here's a list of all commands:```\n";
			constexpr char theEnd[] = "```";
			//estimate length
			std::size_t length = strlen(start) + strlen(theEnd);
			for (Command::MappedCommand& command : Command::all) {
				length += command.first.size();
				length += 2; // ' ' and '\n'
				for (std::string& parmaName : command.second.params) {
					length += 2; // '<' and '> '
					length += parmaName.size();
				}
			}
			
			std::string output;
			output.reserve(length);
			output += start;
			for (Command::MappedCommand& command : Command::all) {
				output += command.first;
				output += ' ';
				for (std::string& parmaName : command.second.params) {
					output += '<';
					output += parmaName;
					output += "> ";
				}
				output += '\n';
			}
			output += theEnd;
			client.sendMessage(message.channelID, output, SleepyDiscord::Async);
		}
	});
	Command::addCommand({
		"square", {"channel"}, 
		createSimpleCommandVerbForVoiceSource<SquareWave>()
	});
	Command::addCommand({
		"noise", {"channel"},
		createSimpleCommandVerbForVoiceSource<Noise>()
	});
	Command::addCommand({
		"music", {"song", "channel"}, [](
			SoundPlayerClient& client,
			SleepyDiscord::Message& message,
			std::queue<std::string>& params
		) {
			std::string songParam = params.front();
			params.pop();
			createSimpleCommandVerbForVoiceSource<RawPCMAudioFile>(songParam)(
				client, message, params
			);
		}
	});
	Command::addCommand({
		"stop", {}, [](
			SoundPlayerClient& client,
			SleepyDiscord::Message& message,
			std::queue<std::string>& params
		) {
			client.disconnectServerVoiceConnections(message.serverID);
		}
	});

	SoundPlayerClient client(token, SleepyDiscord::USER_CONTROLED_THREADS);
	auto intentsList = {
		SleepyDiscord::Intent::SERVER_MESSAGES,	//required for commands via messages
		SleepyDiscord::Intent::SERVER_VOICE_STATES, //required for connecting to voice
	};
	client.setIntents(intentsList);
	client.run();

	return 0;
}