#pragma once

#define SLEEPY_DEFINE_CUSTOM_CLIENT \
namespace SleepyDiscord {\
typedef BaseDiscordClient DiscordClient;\
}

#ifdef SLEEPY_CUSTOM_CLIENT
	#include "client.h"
	SLEEPY_DEFINE_CUSTOM_CLIENT
#elif defined(SLEEPY_DISCORD_CMAKE)
	#if defined(EXISTENT_WEBSOCKETPP)
		#include "websocketpp_websocket.h"
	#elif defined(EXISTENT_UWEBSOCKETS)
		#include "uwebsockets_websocket.h"
	#else
		#include "client.h"
		SLEEPY_DEFINE_CUSTOM_CLIENT
	#endif
#else
	#include "websocketpp_websocket.h"
	#ifdef NONEXISTENT_WEBSOCKETPP
		#include "uwebsockets_websocket.h"
		#ifdef NONEXISTENT_UWEBSOCKETS
			#include "client.h"
			SLEEPY_DEFINE_CUSTOM_CLIENT
		#endif
	#endif
#endif

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

DigiTechs - Today at 12:17 AM
That's a pretty bad way of passing data. I guess if it's written in C it makes sense but you should have written a thin wrapper in C++ for it

I see, you are manually constructing JSON payload too
Sleepy Flower Girl - Today at 2:28 AM
yea
I don't see that as problem yet
qwename - Today at 2:29 AM
Well, it's error-prone, and you might not know the intricacies of JSON enough
TiltMeSenpai - Today at 2:29 AM
there is a json lib for cpp floating around
you want that?
Sleepy Flower Girl - Today at 2:29 AM
I don't need a json lib
qwename - Today at 2:30 AM
The least you can do is pack it into a helper function
Sleepy Flower Girl - Today at 2:30 AM
I understand it enugh that I don't need a lib for that
qwename - Today at 2:31 AM
I pass a dictionary around until I actually need to send the data, then I call a library function to return a JSON payload
The point isn't whether you understand it or not(edited)
TiltMeSenpai - Today at 2:31 AM
ok that's not horrible actually
qwename - Today at 2:31 AM
Right now you have to specify all the curly brackets and commas and whatnot(edited)
NEW MESSAGES
qwename - Today at 2:32 AM
Which takes away time and effort from what you are actually accomplishing
So I suggest the minimum of putting that in a helper method.
I think the map class is an associative array that can do what you need
From <map>

zey - Today at 12:24 AM
i mention someone if
- they'll otherwise miss it
- they're afk
- it's really important
- it's Laura(edited)

*/

namespace SleepyDiscord {
	
}