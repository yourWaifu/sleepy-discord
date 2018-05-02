---
title: How to use voice with Sleepy Discord

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---
[⮌ Go back to documentation](documentation.html)

# Create Applications with Voice with Sleepy Discord

Sleepy Discord can allow applications to use voice channels. This allows for things such as music players or whatever you came up with that can take advantage of Discord's voice chat.

## Connect to a Voice Channel

```cpp
myClient->connectToVoiceChannel("channelID", "serverID");
```

```cpp
myClient->connectToVoiceChannel(myClient->createContext("channelID", "serverID", nullptr));
```

There are a few ways to connect to a voice channel but the first example, calling [BaseDiscordClient::connectToVoiceChannel](documentation.html#connecttovoicechannel) with just a ``channelID`` and ``serverID``, is the simplest.

#### Related Articles
[BaseDiscordClient::connectToVoiceChannel](documentation.html#connecttovoicechannel)

[VoiceContext](documentation.html#voicecontext)

[BaseDiscordClient::createContext](documentation.html#createContext)

## Event Handling
```cpp
class VoiceEventHandler : public SleepyDiscord::BaseVoiceEventHandler {
public:
	VoiceEventHandler() {}
	void onReady(SleepyDiscord::VoiceConnection& connection) {
		/*Do stuff when ready to start sending audio*/
	}
}
VoiceEventHandler voiceEventHandler;

//somewhere else in your code
SleepyDiscord::VoiceContext& context = myClient->createContext("channelID", "serverID", voiceEventHandler);
//or
context.setVoiceHandler(voiceEventHandler);
```

Event handling is done in a separated inherited object, a VoiceEventHandler, creating one is done by simply creating a class that inherits traits from [BaseVoiceEventHandler](documentation.html#basevoiceeventhandler).

During runtime, a reference of or pointer to your VoiceEventHandler will need to be given to a [VoiceContext](documentation.html#voicecontext). This object should live longer then the VoiceContexts that hold the pointer to your VoiceEventHandler (VoiceContexts are destroyed after closing the connection). Not doing so will cause the client to crash.

One useful event being the [BaseVoiceEventHandler::onReady]() event. This is called when the client has finished connecting and is ready to start sending data to one of Discord's voice servers. Some of you maybe confused as to why this is done via a callback and asking "Shouldn't the client be ready after calling [BaseDiscordClient::connectToVoiceChannel](documentation.html#connecttovoicechannel)?". This is because, there's a number of steps involved with getting ready that happen outside the function call.

#### Related Articles
[BaseVoiceEventHandler](documentation.html#basevoiceeventhandler)

[BaseVoiceEventHandler::onReady]()

[VoiceContext::setVoiceHandler](documentation.html#setvoicehandler)

[VoiceConnection](documentation.html#voiceconnection)

## Linking the needed libraries

You'll need those if you want to send or receive audio.

### Sodium

### Opus

### A UDP library

## Sending Audio

Once connected to a voice server with all needed library linked, we can begin sending Audio to over to Discord. To do this, create a [AudioSource](documentation.html#audiosource), and then call [VoiceConnection::startSpeaking](documentation.html#startspeaking).

### Audio Sources
```cpp
struct Music : public SleepyDiscord::AudioSource<SleepyDiscord::AUDIO_POINTER> {
	Music() : SleepyDiscord::AudioSource<SleepyDiscord::AUDIO_POINTER>() {
		File musicFile("music.raw");
		musicLength = musicFile.getSize() / sizeof int16_t;
		music = musicFile.get<int16_t>();
	}
	constexpr inline bool isOpusEncoded() { return false; } //optional, will be false by default
	void read(SleepyDiscord::AudioTransmissionDetails& details, int16_t*& buffer, std::size_t& length) {
		buffer = &music[progress];
		length = (musicLength - progress) < details.proposedLength() ? details.proposedLength() : 0;
		progress += details.proposedLength();
		//note: set length to 0 to stop speaking
	}
	std::size_t progress = 0;
	std::vector<int16_t> music;
	std::size_t musicLength;
};
```

```cpp
struct SquareWave : public SleepyDiscord::AudioSource<SleepyDiscord::AUDIO_VECTOR> {
	SquareWave() : SleepyDiscord::AudioSource<SleepyDiscord::AUDIO_VECTOR>(), sampleOffset(0) {}
	std::vector<int16_t> read(SleepyDiscord::AudioTransmissionDetails& details) {
		std::vector<int16_t> target(details.proposedLength());
		for (int16_t& sample : target) {
				//2000 is the volume
				//100 is how long half the square wave is
				sample = (++sampleOffset / 100) % 2 ? 2000 : -2000;
		}
		return target;
		//note: return vector with a size of 0 to stop speaking
	}
	std::size_t sampleOffset = 0;
};
```

Creating an AudioSource is done by creating a class that inherits a type of AudioSource and filling in the virtual function, ``read``.

This read function should, by default, output PCM stereo audio samples at 48000 Hz with the 2 channels interleaved.
If you aren't familiar with digital audio samples, [here's a good article about it](http://manual.audacityteam.org/man/digital_audio.html).
In the ``read`` function, you'll be given a ``SleepyDiscord::AudioTransmissionDetails``, this will give details about what audio settings to use and some other stuff like how much audio has been sent since last time.
Don't worry about calling read, the library will instead call ``read`` when audio data is needed while sending audio.

> You can also send Opus encoded audio instead of PMC audio

```cpp
struct Music : public SleepyDiscord::AudioSource<SleepyDiscord::AUDIO_POINTER> {
	constexpr inline bool isOpusEncoded() { return true; }
}
```

As of when this was written, there are two types of [AudioSources](documentation.html#audiosource), AUDIO_POINTER and AUDIO_VECTOR. They differ in their read function.

#### AUDIO_POINTER
``read`` points a pointer to a buffer of audio data and sets the length.

#### AUDIO_VECTOR
``read`` returns a vector of audio data

<aside class="note">
The sizes of audio buffers should 960. Which is .02 seconds of 48kHz stereo audio.
</aside>

### Speak
```cpp
//In your VoiceEventHandler
void onReady(SleepyDiscord::VoiceConnection& connection) {
	connection.startSpeaking<SquareWave>(/*Parameters to pass over to SquareWave's constructor*/);
}
```

To start speaking, call [VoiceConnection::startSpeaking](documentation.html#startspeaking) with your AudioSource as the template parameter. To stop, send a buffer with the length of zero in your AudioSource's read function. If your AudioSource has any parameters in it's constructor, you pass them to this function.

#### Related Articles 
[AudioSource](documentation.html#audiosource)

[VoiceConnection::startSpeaking](documentation.html#startspeaking)

## Receiving Audio
Not implemented yet.