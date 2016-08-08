#pragma once
#include "client.h"
#include "message.h"

/*
FEEDBACK
acdenisSK - Today at 8:23 PM
atleast make it parse the json in the function ffs

The Almighty Shubshub - Today at 8:25 PM
Why aren't the message_create events bundled with an isprivate identifier?

Danny - Today at 10:29 PM
1. sleepy_c++_discord_library.h is an awful library name, consider using something a bit more sane like sleepy_discord.hpp or something.
2. C++ style guide is snake_case not ThisCase or thisCase.
3. SleepyDiscord::init is a global function that is a C-ism. C++ we tend to use RAII.
4. Your use of new myEventClass has no notion of ownership.
5. Your parameter jsonMessage has no notion of ownership.
6. Forcing your users to do the heartbeat themselves is poor design.
7. Sleep is not a standard function. std::this_thread::sleep_for(std::chrono::seconds(1)) is though.
8. heartbeat is a global function again which makes me wonder how your design is.
9. Casting to std::string like (std::string)message.author.username is most definitely the wrong thing to do.

based on your code I don't even think you know what modern C++ is
look up C++11/14/17

meh
ownership woes still
and then there's the responsibility of creating the message
also I don't like your message API
std::string already exists.
e.g. message.content.find("whcg hello") == 0 works too
and you could then use message.content with whatever accepts an std::string
for all I know message.content already exists.

it's pretty meh
I don't want some parsed JSON object
if you're gonna use boost.asio use it all the way through
https://github.com/Rapptz/Gears/blob/docs/gears/string/predicate.hpp
*/

namespace SleepyDiscord {
	
}