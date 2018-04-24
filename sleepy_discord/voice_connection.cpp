#ifdef SLEEPY_VOICE_ENABLED
#include <sodium.h>
#include "voice_connection.h"
#include "client.h"

namespace SleepyDiscord {
	void VoiceConnection::disconnect()  {
		if (state & State::CONNECTED)
			origin->disconnect(1000, "", &connection);
		if (heart.isValid())
			heart.stop(); //Kill
		state = static_cast<State>(state & ~State::CONNECTED);
	}

	void VoiceConnection::processMessage(std::string message) {

		origin->onError(ERROR_NOTE, "testing VOICE processMessage");

		//to do there is reused code here, plase place them into functions
		std::vector<std::string> values = json::getValues(message.c_str(),
			{ "op", "d" });
		VoiceOPCode op = static_cast<VoiceOPCode>(std::stoi(values[0]));
		std::string *d = &values[1];
		switch (op) {
		case HELLO: {
			origin->onError(ERROR_NOTE, "testing VOICE HELLO");

			heartbeatInterval = std::stoi(json::getValue(d->c_str(), "heartbeat_interval"));
			std::string identity;
			/*The number 116 comes from the number of letters in this string + 1:
				{"op": 0,"d": {"server_id": "18446744073709551615",
				"user_id": "18446744073709551615","session_id": "","token": ""}}
			*/
			//remember to change the number below when editing identity
			identity.reserve(116 + context.sessionID.length() + origin->token->length());
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
			origin->send(identity, &connection);
			}
			state = static_cast<State>(state | CONNECTED);
			break;
		case READY: {
			origin->onError(ERROR_NOTE, "testing VOICE READY");

			std::vector<std::string> values = json::getValues(d->c_str(),
			{ "ssrc", "port" });
			sSRC = std::stoul(values[0]);
			port = static_cast<uint16_t>(std::stoul(values[1]));
			//start heartbeating
			heartbeat();
			//connect to UDP
			//IP Discovery
			unsigned char packet[70] = { 0 };
			packet[0] = (sSRC >> 24) & 0xff;
			packet[1] = (sSRC >> 16) & 0xff;
			packet[2] = (sSRC >>  8) & 0xff;
			packet[3] = (sSRC      ) & 0xff;
			UDPClient::send(context.endpoint, port, packet, 70);
			const std::vector<uint8_t> iPDiscovery = UDPClient::receive(context.endpoint, port); //note: receive blocks and is not async
			//find start of string. 0x60 is a bitmask that should filter out non-letters
			std::vector<uint8_t>::const_iterator iPStart = iPDiscovery.begin() + 2;
			const std::string iPAddress(iPStart, std::find(iPStart, iPDiscovery.end(), 0)); //find may not be needed
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
			origin->send(protocol, &connection);
			}
			state = static_cast<State>(state | State::OPEN);
			break;
		case SESSION_DESCRIPTION:
			origin->onError(ERROR_NOTE, "testing VOICE SESSION_DESCRIPTION");

			{
			std::vector<std::string> stringArray = json::getArray(&json::getValue(d->c_str(), "secret_key"));
			const unsigned int stringArraySize = stringArray.size();
			for (unsigned int i = 0; i < SECRET_KEY_SIZE && i < stringArraySize; ++i) {
				if (stringArray[i] != "")
					secretKey[i] = std::stoul(stringArray[i]) & 0xFF;
			}
			}
			state = static_cast<State>(state | State::AUDIO_ENABLED);
			if (context.eventHandler != nullptr)
				context.eventHandler->onReady(*this);
			break;
		case RESUMED:
			origin->onError(ERROR_NOTE, "testing VOICE RESUMED");
			break;
		case HEARTBEAT_ACK:
			origin->onError(ERROR_NOTE, "testing VOICE HEARTBEAT_ACK");
			break;
		default:
			origin->onError(ERROR_NOTE, "testing VOICE default");
			break;
		}
	}

	void VoiceConnection::heartbeat() {

		origin->onError(ERROR_NOTE, "testing VOICE heartbeat");

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
		origin->send(heartbeat, &connection);

		heart = origin->schedule([this]() {
			this->heartbeat();
		}, heartbeatInterval);
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
			if (!(state & CAN_ENCODE)) {
				//init opus
				int opusError = 0;
				encoder = opus_encoder_create(
					/*Sampling rate(Hz)*/48000,
					/*Channels*/         2,
					/*Mode*/             OPUS_APPLICATION_VOIP,
					&opusError);
				if (opusError) {//error check
					return;
				}
				//opusError = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(BITRATE));
				if (opusError) {
					return;
				}
				state = static_cast<State>(state | State::CAN_ENCODE);
			}
#endif

		//say something
		sendSpeaking(true);
		speak();
	}

	void VoiceConnection::sendSpeaking(bool isNowSpeaking) {
		std::string ssrc = std::to_string(sSRC);
		/*The number 49 comes from 1 plus the length of this string
			{"op":5,"d":{"speaking":false,"delay":0,"ssrc":}}
		*/
		std::string speaking;
		speaking.reserve(49 + ssrc.length());
		speaking +=
			"{"
				"\"op\":5,"
				"\"d\":{"
					"\"speaking\":"; speaking += json::boolean(isNowSpeaking); speaking += ","
					"\"delay\":0,"
					"\"ssrc\":"; speaking += ssrc; speaking +=
				"}"
			"}";
		origin->send(speaking, &connection);
	}

	void VoiceConnection::speak() {
		//check that we are can still send audio data
		if ((state & State::ABLE) != State::ABLE)
			return;

		AudioTransmissionDetails details(
			context, 
			static_cast<std::size_t>(48000 * 2 * .020),
			samplesSentLastTime
		);

		//send the audio data
		switch (audioSource->type) {
		case AUDIO_VECTOR:
		case AUDIO_VECTOR_S16: {
			auto audioVectorSource = &static_cast<AudioSource<AUDIO_VECTOR>&>(*audioSource);
			std::vector<int16_t> audioData = audioVectorSource->read(details);
			int16_t* audioBuffer = audioData.data();
			speak(audioBuffer, audioData.size());
		} break;
		case AUDIO_POINTER:
		case AUDIO_POINTER_S16: {
			auto audioVectorSource = &static_cast<AudioSource<AUDIO_POINTER>&>(*audioSource);
			int16_t * audioData = nullptr;
			std::size_t length = 0;
			audioVectorSource->read(details, audioData, length);
			speak(audioData, length);
		} break;
		default:
			break;
		}
	}

	void VoiceConnection::speak(int16_t*& audioData, const std::size_t & length)  {
		samplesSentLastTime = 0;
		const time_t startTime = origin->getEpochTimeMillisecond();
		//This is only called in speak() so already checked that we can still send audio data

		//stop sending data when there's no data
		if (length == 0) {
			sendSpeaking(false);
			return;
		}

		//the >>1 cuts it in half since you are using 2 channels
		const std::size_t frameSize = length >> 1;

		if (!audioSource->isOpusEncoded()) {
#if defined(NONEXISTENT_OPUS)
			return;
#else
			//encode data
			constexpr opus_int32 encodedAudioMaxLength = 960;
			unsigned char encodedAudioData[encodedAudioMaxLength];
			opus_int32 encodedAudioLength = opus_encode(
				encoder, audioData, frameSize,
				encodedAudioData, encodedAudioMaxLength);
			//send it
			uint8_t * encodedAudioDataPointer = encodedAudioData;
			sendAudioData(encodedAudioDataPointer, encodedAudioLength, frameSize);
#endif
		} else {
			//encoded data should be in uint8
			sendAudioData(reinterpret_cast<uint8_t*&>(audioData), length, frameSize);
		}

		//schedule next send
		const time_t interval = static_cast<time_t>(
			(static_cast<float>(frameSize) / (48000.0f)) * 1000.0f
		);
		const time_t nextTime = startTime + interval;
		const time_t delay = nextTime - origin->getEpochTimeMillisecond();

		origin->schedule([this]() {
			this->speak();
		}, 0 < delay ? delay : 0); //I think 20 is the frame length
	}

	void VoiceConnection::sendAudioData(
		uint8_t*& encodedAudioData,
		const std::size_t & length,
		const std::size_t & frameSize
	) {
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
			
		uint8_t nonce[24];
		std::memcpy(nonce                , header, sizeof nonce - sizeof header);
		std::memset(nonce + sizeof header,      0, sizeof nonce - sizeof header);
		
		const size_t numOfBtyes = sizeof header + length + crypto_secretbox_MACBYTES;
		std::vector<uint8_t> audioDataPacket(numOfBtyes);
		std::memcpy(audioDataPacket.data(), header, sizeof header);

		crypto_secretbox_easy(audioDataPacket.data() + sizeof header,
			encodedAudioData, length, nonce, secretKey);

		UDPClient::send(context.endpoint, port, audioDataPacket.data(), audioDataPacket.size());
		samplesSentLastTime = frameSize << 1;
		timestamp += frameSize;
	}
}
#endif
