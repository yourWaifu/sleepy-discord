#pragma once
#include <vector>
#include <array>
#include <cstdint>
#include <list>
#if (!defined(NONEXISTENT_OPUS) && !defined(SLEEPY_DISCORD_CMAKE)) || defined(EXISTENT_OPUS)
#include <opus.h>
#endif
#include "udp_client.h"
#include "snowflake.h"
#include "server.h"
#include "channel.h"
#include "message_receiver.h"
#include "timer.h"

namespace SleepyDiscord {
	using AudioSample = int16_t;

	class BaseDiscordClient;
	class VoiceConnection;

	class BaseVoiceEventHandler {
	public:
		virtual ~BaseVoiceEventHandler() = default;
		virtual void onReady(VoiceConnection&) {}
		virtual void onSpeaking(VoiceConnection&) {}
		virtual void onEndSpeaking(VoiceConnection&) {}
		virtual void onFinishedSpeaking(VoiceConnection&) {}
		virtual void onHeartbeat(VoiceConnection&) {}
		virtual void onHeartbeatAck(VoiceConnection&) {}
	};

	struct VoiceContext {
		friend VoiceConnection;
		friend BaseDiscordClient;
	public:
		inline Snowflake<Channel> getChannelID() {
			return channelID;
		}

		inline Snowflake<Server> getServerID() {
			return serverID;
		}

		inline bool operator==(const VoiceContext& right) {
			return this == &right;
		}

		inline void setVoiceHandler(BaseVoiceEventHandler* source) {
			eventHandler = std::unique_ptr<BaseVoiceEventHandler>(source);
		}

		inline bool hasVoiceHandler() {
			return eventHandler != nullptr;
		}

		inline BaseVoiceEventHandler& getVoiceHandler() {
			return *(eventHandler.get());
		}

		template<class EventHandler, class... Types>
		inline void startVoiceHandler(Types&&... arguments) {
			setVoiceHandler(new EventHandler(std::forward<Types>(arguments)...));
		}

	private:
		VoiceContext(Snowflake<Server> _serverID, Snowflake<Channel> _channelID, BaseVoiceEventHandler* _eventHandler) :
			serverID(_serverID), channelID(_channelID), eventHandler(_eventHandler)
		{}

		Snowflake<Server> serverID;
		Snowflake<Channel> channelID;
		std::string sessionID = "";
		std::string endpoint = "";
		std::string token;
		std::unique_ptr<BaseVoiceEventHandler> eventHandler;
	};

	enum AudioSourceType {
		AUDIO_BASE_TYPE,
		AUDIO_CONTAINER,
	};

	class VoiceConnection;

	struct AudioTransmissionDetails {
	public:
		inline VoiceContext& context() {
			return _context;
		}

		inline std::size_t amountSentSinceLastTime() {
			return _amountSentSinceLastTime;
		}

		static inline constexpr int bitrate() {
			return 48000;
		}

		static inline constexpr int channels() {
			return 2;
		}

		static inline constexpr std::size_t proposedLengthOfTime() {
			return 20;
		}

		static inline constexpr std::size_t proposedLength() {
			return static_cast<std::size_t>(
				bitrate() * channels() * (
					static_cast<float>(proposedLengthOfTime()) / 1000 /*millisecond conversion*/
				)
			);
		}

	private:
		friend VoiceConnection;
		AudioTransmissionDetails(
			VoiceContext& con,
			const std::size_t amo
		) :
			_context(con),
			_amountSentSinceLastTime(amo)
		{ }

		VoiceContext& _context;
		const std::size_t _amountSentSinceLastTime;
	};

	struct BaseAudioSource {
		BaseAudioSource() : type(AUDIO_BASE_TYPE) {}
		explicit BaseAudioSource(AudioSourceType typ) : type(typ) {}
		virtual inline bool isOpusEncoded() { return false; }
		const AudioSourceType type;
		virtual ~BaseAudioSource() = default;
		//This function below is here in case the user uses this class
		virtual void read(AudioTransmissionDetails& /*details*/, int16_t*& /*buffer*/, std::size_t& /*length*/) {};

		enum SpeakingFlag : unsigned int {
			Microphone = 1u << 0u,
			Soundshare = 1u << 1u,
			Priority = 1u << 2u,
		};
		SpeakingFlag speakingFlag = Microphone;
	};

	struct BaseAudioOutput {
		using Container = std::array<AudioSample, AudioTransmissionDetails::proposedLength()>;
		BaseAudioOutput() = default;
		virtual ~BaseAudioOutput() = default;
		virtual void write(Container audio, AudioTransmissionDetails& details) {}
	private:
		friend VoiceConnection;
	};

	struct AudioTimer {
		Timer timer;
		time_t nextTime = 0;
		void stop() {
			if (timer.isValid())
				timer.stop();
		}
	};

	class VoiceConnection : public GenericMessageReceiver {
	public:
		VoiceConnection(BaseDiscordClient* client, VoiceContext& _context);
		VoiceConnection(VoiceConnection&&) = default;

		~VoiceConnection() = default;

		inline bool operator==(const VoiceConnection& right) {
			return this == &right;
		}

		inline bool isReady() const {
			return state & State::ABLE;
		}

		inline void setAudioSource(BaseAudioSource*& source) {
			audioSource = std::unique_ptr<BaseAudioSource>(source);
		}

		inline bool hasAudioSource() const {
			return audioSource != nullptr;
		}

		inline BaseAudioSource& getAudioSource() {
			return *audioSource;
		}

		/*To do there might be a way to prevent code reuse here*/

		inline void setAudioOutput(BaseAudioOutput*& output) {
			audioOutput = std::unique_ptr<BaseAudioOutput>(output);
		} 

		inline bool hasAudioOutput() const {
			return audioOutput != nullptr;
		}

		inline BaseAudioOutput& getAudioOutput() {
			return *audioOutput;
		}

		//=== startSpeaking functions ===

		void startSpeaking();

		inline void startSpeaking(BaseAudioSource* source) {
			setAudioSource(source);
			startSpeaking();
		}

		template<class AudioSource, class... Types>
		inline void startSpeaking(Types&&... arguments) {
			startSpeaking(new AudioSource(std::forward<Types>(arguments)...));
		}

		//=== startListening ===

		void startListening();

		inline BaseDiscordClient& getDiscordClient() {
			return *origin;
		}

		inline BaseDiscordClient& getOrigin() {
			return getDiscordClient();
		}

		inline VoiceContext& getContext() {
			return context;
		}

		void speak(AudioSample*& audioData, const std::size_t& length);

		void disconnect();

		//Discord doens't gives the endpoint with wss:// or ?v=3, so it's done here
		static std::string getWebSocketURI(const std::string& givenEndpoint) {
			std::string endpoint;
			//length of wss:///?v=3 is 11, plus one equals 12
			endpoint.reserve(12 + givenEndpoint.length());
			endpoint += "wss://";
			endpoint += givenEndpoint;
			endpoint += "/?v=3";
			return endpoint;
		}
	private:
		friend BaseDiscordClient;

		void initialize() override;
		void processMessage(const std::string &message) override;
		void processCloseCode(const int16_t code) override;

		enum VoiceOPCode {
			IDENTIFY            = 0,  //client begin a voice websocket connection
			SELECT_PROTOCOL     = 1,  //client select the voice protocol
			READY               = 2,  //server complete the websocket handshake
			HEARTBEAT           = 3,  //client keep the websocket connection alive
			SESSION_DESCRIPTION = 4,  //server describe the session
			SPEAKING            = 5,  //both   indicate which users are speaking
			HEARTBEAT_ACK       = 6,  //server sent immediately following a received client heartbeat
			RESUME              = 7,  //client resume a connection
			HELLO               = 8,  //server the continuous interval in milliseconds after which the client should send a heartbeat
			RESUMED             = 9,  //server acknowledge Resume
			CLIENT_DISCONNECT   = 13  //server a client has disconnected from the voice channel
		};
			
		enum State : uint8_t {
			NOT_CONNECTED = 0 << 0,
			CONNECTED     = 1 << 0,
			OPEN          = 1 << 1,
			AUDIO_ENABLED = 1 << 2,
			SENDING_AUDIO = 1 << 3,

			CAN_ENCODE    = 1 << 6,
			CAN_DECODE    = 1 << 7,

			ABLE          = CONNECTED | OPEN | AUDIO_ENABLED,
		};

#ifdef NONEXISTENT_OPUS
		using OpusEncoder = void;
		using OpusDecoder = void;
#endif

		BaseDiscordClient* origin;
		VoiceContext& context;
		UDPClient UDP;
		time_t heartbeatInterval = 0;
		uint32_t sSRC;
		uint16_t port;
		Timer heart;
		State state = State::NOT_CONNECTED;
		int16_t numOfPacketsSent = 0;
		std::unique_ptr<BaseAudioSource> audioSource;
		std::unique_ptr<BaseAudioOutput> audioOutput;
		AudioTimer speechTimer;
		AudioTimer listenTimer;
		std::size_t samplesSentLastTime = 0;
		time_t nextTime = 0;
		OpusEncoder *encoder = nullptr;
		OpusDecoder *decoder = nullptr;
		uint16_t sequence = 0;
		uint32_t timestamp = 0;

		std::array<unsigned char, 32> secretKey;
		static constexpr int nonceSize = 24;

		//to do use this for events
		template<class... Types>
		inline void callEvent(void (BaseVoiceEventHandler::*member)(Types...), Types&&... arguments){
			if(context.eventHandler != nullptr)
				((*context.eventHandler).*member)(arguments...);
		}
		void heartbeat();
		inline void scheduleNextTime(AudioTimer& timer, TimedTask code, const time_t interval);
		inline void stopSpeaking() {
			state = static_cast<State>(state & ~SENDING_AUDIO);
		}
		void sendSpeaking(bool isNowSpeaking);
		void speak();
		void sendAudioData(
			uint8_t*& encodedAudioData,
			const std::size_t & length,
			const std::size_t & frameSize
		);
		void listen();
		void processIncomingAudio(const std::vector<uint8_t>& data);
	};

	struct BasicAudioSourceForContainers : public BaseAudioSource {
		BasicAudioSourceForContainers() : BaseAudioSource(AUDIO_CONTAINER) {}
		virtual void speak(
			VoiceConnection& connection,
			AudioTransmissionDetails& details,
			std::size_t& length
		) = 0;
	};

	template<class _Container>
	struct AudioSource : public BasicAudioSourceForContainers {
	public:
		using Container = _Container;
		AudioSource() : BasicAudioSourceForContainers() {}
		virtual void read(AudioTransmissionDetails& /*details*/, int16_t*& /*buffer*/, std::size_t& /*length*/)  override {};
		virtual void read(AudioTransmissionDetails& details, Container& target) {};
	private:
		friend VoiceConnection;
		void speak(
			VoiceConnection& connection,
			AudioTransmissionDetails& details,
			std::size_t& length
		) override {
			read(details, containedAudioData);
			int16_t* audioBuffer = containedAudioData.data();
			length = containedAudioData.size();
			connection.speak(audioBuffer, length);
		}
	protected:
		Container containedAudioData;
	};

	struct AudioVectorSource : public AudioSource<std::vector<AudioSample>> {
	public:
		AudioVectorSource() : AudioSource<std::vector<AudioSample>>() {
			containedAudioData.resize(AudioTransmissionDetails::proposedLength());
		}
	};

	using AudioPointerSource = BaseAudioSource;
}