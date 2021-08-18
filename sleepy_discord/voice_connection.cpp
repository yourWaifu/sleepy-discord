#include "voice_connection.h"
#ifdef SLEEPY_VOICE_ENABLED
#include <sodium.h>
#include "client.h"

namespace SleepyDiscord {
	VoiceConnection::VoiceConnection(BaseDiscordClient* client, VoiceContext& _context) :
		origin(client), context(_context), UDP(*origin), sSRC(0), port(0), nextTime(0),
#if !defined(NONEXISTENT_OPUS)
		encoder(nullptr), decoder(nullptr),
#endif
		secretKey()
	{}

	void VoiceConnection::disconnect() {
		stopSpeaking();
		std::string update;
		/*The number 103 comes from the number of letters in this string + 1:
		{"op":4,"d":{"guild_id":"18446744073709551615","channel_id":null,"self_mute":false,"self_deaf":false}}
		*/
		update.reserve(103);
		update +=
			"{"
				"\"op\":4,"
				"\"d\":{"
					"\"guild_id\":\""; update += context.serverID; update += "\","
					"\"channel_id\":null,"
					"\"self_mute\":false,"
					"\"self_deaf\":false"
				"}"
			"}";
		origin->send(update, origin->connection);

		State oldState = state;
		state = static_cast<State>(state & ~State::CONNECTED);

		if (oldState & State::CONNECTED)
			origin->disconnect(1000, "", connection);
		if (heart.isValid())
			heart.stop(); //Kill
		speechTimer.stop();
		listenTimer.stop();
		//deal with raw pointers
		//Sorry about this c code, we are dealing with c libraries
#ifndef NONEXISTENT_OPUS
		if (encoder != nullptr) {
			opus_encoder_destroy(encoder);
			encoder = nullptr;
		}
		if (decoder != nullptr) {
			opus_decoder_destroy(decoder);
			decoder = nullptr;
		}
#endif // !NONEXISTENT_OPUS
	}

	void VoiceConnection::initialize() {
		if (state == NOT_CONNECTED)
			return;

		std::string resume;
		/*The number 77 comes from the number of letters in this string + 1:
		{"op":7,"d":{"server_id":"18446744073709551615","session_id":"","token":""}}
		*/
		resume.reserve(77 + context.sessionID.length() + context.token.length());
		resume +=
			"{"
				"\"op\":7," //RESUME
				"\"d\":{"
					"\"server_id\":\"" ; resume += context.serverID ; resume += "\","
					"\"session_id\":\""; resume += context.sessionID; resume += "\","
					"\"token\":\""     ; resume += context.token    ; resume += "\""
				"}"
			"}";
		origin->send(resume, origin->connection);
	}

	void VoiceConnection::processMessage(const std::string &message) {
		//json::Values values = json::getValues(message.c_str(),
		//	{ "op", "d" });
		rapidjson::Document values;
		values.Parse(message.c_str(), message.length());

		VoiceOPCode op = static_cast<VoiceOPCode>(json::toInt(values["op"]));
		json::Value& d = values["d"];
		switch (op) {
		case HELLO: {
			heartbeatInterval = d["heartbeat_interval"].GetInt();

			//Don't sent a identity during resumes
			if (state & OPEN)
				break;

			std::string identity;
			/*The number 116 comes from the number of letters in this string + 1:
				{"op": 0,"d": {"server_id": "18446744073709551615",
				"user_id": "18446744073709551615","session_id": "","token": ""}}
			*/
			//remember to change the number below when editing identity
			identity.reserve(116 + context.sessionID.length() + context.token.length());
			identity +=
				"{"
					"\"op\": 0," //VoiceOPCode::IDENTIFY
					"\"d\": {"
						"\"server_id\": \"" ; identity += context.serverID ; identity += "\","
						"\"user_id\": \""   ; identity += origin->getID()  ; identity += "\","
						"\"session_id\": \""; identity += context.sessionID; identity += "\","
						"\"token\": \""     ; identity += context.token    ; identity += "\""
					"}"
				"}";
			origin->send(identity, connection);
			}
			state = static_cast<State>(state | CONNECTED);
			break;
		case READY: {
			//json::Values values = json::getValues(d->c_str(),
			//{ "ssrc", "port" });
			sSRC = d["ssrc"].GetUint();
			port = static_cast<uint16_t>(d["port"].GetUint());
			const json::Value& ipValue = d["ip"];
			std::string ip(ipValue.GetString(), ipValue.GetStringLength());
			//start heartbeating
			heartbeat();
			//connect to UDP
			UDP.connect(ip, port);
			//IP Discovery
			unsigned char packet[70] = { 0 };
			packet[0] = (sSRC >> 24) & 0xff;
			packet[1] = (sSRC >> 16) & 0xff;
			packet[2] = (sSRC >>  8) & 0xff;
			packet[3] = (sSRC      ) & 0xff;
			UDP.send(packet, 70);
			UDP.receive([&](const std::vector<uint8_t>& iPDiscovery) {
				//find start of string. 0x60 is a bitmask that should filter out non-letters
				//the ip is in ascii starting with the 4th byte and is null terminated
				std::vector<uint8_t>::const_iterator iPStart = iPDiscovery.begin() + 4;
				const std::string iPAddress(iPStart, std::find(iPStart, iPDiscovery.end(), 0));
				//send Select Protocol Payload
				std::string protocol;
				/*The number 101 comes from the number of letters in this string + 1:
					{"op": 1,"d": {"protocol": "udp","data": {
					"address": "","port": 65535,
					"mode": "xsalsa20_poly1305"}}}
				*/
				protocol.reserve(101 + iPAddress.length());
				protocol +=
					"{"
						"\"op\": 1," //VoiceOPCode::SELECT_PROTOCOL
						"\"d\": {"
							"\"protocol\": \"udp\","
							"\"data\": {"
								"\"address\": \""; protocol += iPAddress           ; protocol += "\","
								"\"port\": "     ; protocol += std::to_string(port); protocol +=   ","
								"\"mode\": \"xsalsa20_poly1305\""
							"}"
						"}"
					"}";
				origin->send(protocol, connection);
			});
			}
			state = static_cast<State>(state | State::OPEN);
			break;
		case SESSION_DESCRIPTION: {
			consecutiveReconnectsCount = 0;  //succusful connection
			const json::Value& secretKeyJSON = d["secret_key"];
			json::Array secretKeyJSONArray = secretKeyJSON.GetArray();
			const std::size_t secretKeyJSONArraySize = secretKeyJSONArray.Size();
			for (std::size_t i = 0; i < secretKey.max_size() && i < secretKeyJSONArraySize; ++i) {
					secretKey[i] = secretKeyJSONArray[i].GetUint() & 0xFF;
			}
			}
			state = static_cast<State>(state | State::AUDIO_ENABLED);
			if (context.eventHandler != nullptr)
				context.eventHandler->onReady(*this);
			break;
		case SPEAKING:
			if (context.eventHandler != nullptr)
				context.eventHandler->onSpeaking(*this);
		case RESUMED:
			consecutiveReconnectsCount = 0;
			heartbeat();
			break;
		case HEARTBEAT_ACK:
			if (context.eventHandler != nullptr)
				context.eventHandler->onHeartbeatAck(*this);
			break;
		default:
			break;
		}
	}

	void VoiceConnection::processCloseCode(const int16_t code) {
		State oldState = state;
		state = static_cast<State>(state & ~State::CONNECTED);

		switch (code) {
		case 1000: //normal closure
		case 1001:
		case VOICE_SERVER_CRASHED:
			if (oldState & State::CONNECTED)
				disconnect();
			getDiscordClient().removeVoiceConnectionAndContext(*this);
			return;
		default: break;
		}

		if (heart.isValid())
			heart.stop(); //Kill

		if (reconnectTimer.isValid()) //overwrite reconnect timer
			reconnectTimer.stop();
		reconnectTimer = origin->schedule([this]() {
			origin->connect(getWebSocketURI(context.endpoint), this, connection);
		}, getRetryDelay());
		++consecutiveReconnectsCount;
	}

	void VoiceConnection::heartbeat() {
		//don't continue if not connected
		if (!(state & CONNECTED))
			return;

		//timestamp int
		const uint64_t bitMask52 = 0x1FFFFFFFFFFFFF;
		const uint64_t currentTime = static_cast<uint16_t>(origin->getEpochTimeMillisecond());
		const std::string nonce = std::to_string(bitMask52 & currentTime);
		/*The number 17 comes from the number of letters in this string + 1:
		{"op": 3, "d": }
		*/
		std::string heartbeat;
		heartbeat.reserve(17 + nonce.length());
		heartbeat += 
			"{"
				"\"op\": 3, "
				"\"d\": "; heartbeat += nonce; heartbeat += 
			'}';
		origin->send(heartbeat, connection);

		if (context.eventHandler != nullptr)
			context.eventHandler->onHeartbeat(*this);

		heart = origin->schedule([this]() {
			this->heartbeat();
		}, heartbeatInterval);
	}

	inline void VoiceConnection::scheduleNextTime(AudioTimer& timer, TimedTask code, const time_t interval) {
		timer.nextTime += interval;
		time_t delay = timer.nextTime - origin->getEpochTimeMillisecond();
		delay = 0 < delay ? delay : 0;

		timer.timer = origin->schedule(code, delay);
	}

	void VoiceConnection::startSpeaking() {
		if ((state & State::ABLE) != State::ABLE) return;

		//init libraries if havn't and not encoded data
		//init sodium
		if (sodium_init() < 0)
			return;

		if (!audioSource->isOpusEncoded())
#if defined(NONEXISTENT_OPUS)
			return;
#else
			if (!(state & CAN_ENCODE) || encoder == nullptr) {
				//init opus
				int opusError = 0;
				encoder = opus_encoder_create(
					/*Sampling rate(Hz)*/AudioTransmissionDetails::bitrate(),
					/*Channels*/         AudioTransmissionDetails::channels(),
					/*Mode*/             OPUS_APPLICATION_VOIP,
					&opusError);
				if (opusError) {//error check
					return;
				}
				state = static_cast<State>(state | State::CAN_ENCODE);
			}
#endif

		//say something
		sendSpeaking(true);
		state = static_cast<State>(state | State::SENDING_AUDIO);
		speechTimer.nextTime = origin->getEpochTimeMillisecond();
		speak();
	}

	void VoiceConnection::sendSpeaking(bool isNowSpeaking) {
		std::string ssrc = std::to_string(sSRC);
		/*The number 49 comes from 1 plus the length of this string
			{"op":5,"d":{"speaking":false,"delay":0,"ssrc":}}
		*/
		std::string speaking;
		BasicAudioSourceForContainers::SpeakingFlag speakingFlag =
			isNowSpeaking ? audioSource->speakingFlag :
			static_cast< BasicAudioSourceForContainers::SpeakingFlag>(0);
		speaking.reserve(49 + ssrc.length());
		speaking +=
			"{"
				"\"op\":5,"
				"\"d\":{"
					"\"speaking\":"; speaking += json::integer(speakingFlag); speaking += ","
					"\"delay\":0,"
					"\"ssrc\":"; speaking += ssrc; speaking +=
				"}"
			"}";
		origin->send(speaking, connection);
	}

	void VoiceConnection::speak() {
		//check that we are can still send audio data
		if ((state & State::ABLE) != State::ABLE)
			return;

		AudioTransmissionDetails details(context, samplesSentLastTime);

		std::size_t length = 0;

		//send the audio data
		if (audioSource->type == AUDIO_CONTAINER) {
			auto audioVectorSource = &static_cast<BasicAudioSourceForContainers&>(*audioSource);
			audioVectorSource->speak(*this, details, length);
		} else {
			AudioSample* audioBuffer = nullptr;
			audioSource->read(details, audioBuffer, length);
			speak(audioBuffer, length);
		}

		if ((state & SENDING_AUDIO) == 0) {
			sendSpeaking(false);
			context.eventHandler->onEndSpeaking(*this);
			return;
		}

		//schedule next send
		const time_t interval = static_cast<time_t>(
			(static_cast<float>(length) / static_cast<float>(
				AudioTransmissionDetails::bitrate() * AudioTransmissionDetails::channels()
			)) * 1000.0f
		);

		scheduleNextTime(speechTimer,
			[this]() {
				this->speak();
			}, interval
		);
	}

	void VoiceConnection::speak(AudioSample*& audioData, const std::size_t & length)  {
		samplesSentLastTime = 0;
		//This is only called in speak() so already checked that we can still send audio data

		//stop sending data when there's no data
		if (length == 0) {
			return stopSpeaking();
		} else if ((state & SENDING_AUDIO) == 0) {
			return;
		}

		//the >>1 cuts it in half since you are using 2 channels
		const std::size_t frameSize = length >> 1;

		if (!audioSource->isOpusEncoded()) {
#if defined(NONEXISTENT_OPUS)
			return;
#else
			//encode data
			constexpr opus_int32 encodedAudioMaxLength =
				static_cast<opus_int32>(AudioTransmissionDetails::proposedLength());
			unsigned char encodedAudioData[encodedAudioMaxLength]; //11.52 kilobytes
			opus_int32 encodedAudioLength = opus_encode(
				encoder, audioData, static_cast<int>(frameSize),
				encodedAudioData, encodedAudioMaxLength);
			//send it
			uint8_t * encodedAudioDataPointer = encodedAudioData;
			sendAudioData(encodedAudioDataPointer, encodedAudioLength, frameSize);
#endif
		} else {
			//encoded data should be in uint8
			sendAudioData(reinterpret_cast<uint8_t*&>(audioData), length, frameSize);
		}
	}

	void VoiceConnection::sendAudioData(
		uint8_t*& encodedAudioData,
		const std::size_t & length,
		const std::size_t & frameSize
	) {
#ifndef NONEXISTENT_SODIUM
		++sequence;
		constexpr int headerSize = 12;

		const uint8_t header[headerSize] = {
			0x80,
			0x78,
			static_cast<uint8_t>((sequence  >> (8 * 1)) & 0xff),
			static_cast<uint8_t>((sequence  >> (8 * 0)) & 0xff),
			static_cast<uint8_t>((timestamp >> (8 * 3)) & 0xff),
			static_cast<uint8_t>((timestamp >> (8 * 2)) & 0xff),
			static_cast<uint8_t>((timestamp >> (8 * 1)) & 0xff),
			static_cast<uint8_t>((timestamp >> (8 * 0)) & 0xff),
			static_cast<uint8_t>((sSRC      >> (8 * 3)) & 0xff),
			static_cast<uint8_t>((sSRC      >> (8 * 2)) & 0xff),
			static_cast<uint8_t>((sSRC      >> (8 * 1)) & 0xff),
			static_cast<uint8_t>((sSRC      >> (8 * 0)) & 0xff),
		};
			
		uint8_t nonce[nonceSize];
		std::memcpy(nonce                , header, sizeof header);
		std::memset(nonce + sizeof header,      0, sizeof nonce - sizeof header);
		
		const size_t numOfBtyes = sizeof header + length + crypto_secretbox_MACBYTES;
		std::vector<uint8_t> audioDataPacket(numOfBtyes);
		std::memcpy(audioDataPacket.data(), header, sizeof header);

		crypto_secretbox_easy(audioDataPacket.data() + sizeof header,
			encodedAudioData, length, nonce, secretKey.data());

		UDP.send(audioDataPacket.data(), audioDataPacket.size());
		samplesSentLastTime = frameSize << 1;
		timestamp += static_cast<uint32_t>(frameSize);
#else
	#error Can not use voice without libsodium, libsodium not detected.
#endif
	}

	//To do test this
	void VoiceConnection::startListening() {
		if (!(state & CAN_DECODE) || decoder == nullptr) {
			int opusError = 0;
			decoder = opus_decoder_create(
				/*Sampling rate(Hz)*/AudioTransmissionDetails::bitrate(),
				/*Channels*/         AudioTransmissionDetails::channels(),
				&opusError);
			if (opusError) {//error check
				return;
			}
		}
		listen();
	}

	void VoiceConnection::listen() {
		UDP.receive([this](const std::vector<uint8_t>& data){
			processIncomingAudio(data);
		});

		scheduleNextTime(listenTimer,
			[this]() {
				this->listen();
			},  AudioTransmissionDetails::proposedLengthOfTime()
		);
	}

	void VoiceConnection::processIncomingAudio(const std::vector<uint8_t>& data)
	{
#if !defined(NONEXISTENT_SODIUM) || !defined(NONEXISTENT_OPUS)
		//get nonce
		uint8_t nonce[nonceSize];
		std::memcpy(nonce, data.data(), sizeof nonce);
		//decrypt
		std::vector<uint8_t> decryptedData;
		const std::size_t decryptedDataSize = data.size() - sizeof nonce;
		decryptedData.reserve(decryptedDataSize);
		bool isForged = crypto_secretbox_open_easy(
			decryptedData.data(),
			data.data() + sizeof nonce,
			decryptedDataSize, nonce, secretKey.data()
		) != 0;
		if (isForged)
			return;
		//decode
		constexpr opus_int32 frameSize = 
			static_cast<opus_int32>(AudioTransmissionDetails::proposedLength());
		BaseAudioOutput::Container decodedAudioData;
		opus_int32 decodedAudioLength = opus_decode(
			decoder, decryptedData.data(), static_cast<int>(decryptedData.size()),
			decodedAudioData.data(), frameSize, 1);
		if(decodedAudioLength < OPUS_OK || !hasAudioOutput())
			return;
		AudioTransmissionDetails details(context, 0);
		audioOutput->write(decodedAudioData, details);
#endif
	}
}
#else
void SleepyDiscord::VoiceConnection::initialize() {}
void SleepyDiscord::VoiceConnection::processMessage(const std::string &/*message*/) {}
void SleepyDiscord::VoiceConnection::processCloseCode(const int16_t /*code*/) {}
#endif
