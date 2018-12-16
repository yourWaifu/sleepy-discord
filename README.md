# Sleepy Discord
C++ library for Discord

Note: Sleepy Discord is pronounced as CP Discord, like the letters C and P. Actually, that's a joke. Sleepy should be pronounced the way you would normally pronounce sleepy.

## Build Status
| OS | Windows | Ubuntu
| ------ | ------- | ------
| Master   | [![Build Status](https://dev.azure.com/wuhao64/sleepy-discord/_apis/build/status/yourWaifu.sleepy-discord?branchName=master)](https://dev.azure.com/wuhao64/sleepy-discord/_build/?definitionId=2) | [![Build Status](https://travis-ci.org/yourWaifu/sleepy-discord.svg?branch=master)](https://travis-ci.org/yourWaifu/sleepy-discord)
| Develop   | [![Build Status](https://dev.azure.com/wuhao64/sleepy-discord/_apis/build/status/yourWaifu.sleepy-discord?branchName=develop)](https://dev.azure.com/wuhao64/sleepy-discord/_build/latest?definitionId=2) | [![Build Status](https://travis-ci.org/yourWaifu/sleepy-discord.svg?branch=develop)](https://travis-ci.org/yourWaifu/sleepy-discord)

# [Documentation](https://yourWaifu.github.io/sleepy-discord/documentation.html)
[:pencil:](https://github.com/yourWaifu/sleepy-discord/edit/docs/source/compile.html.md "Edit Documentation") [How to Compile](https://yourwaifu.github.io/sleepy-discord/compile.html)<br />
[:pencil:](https://github.com/yourWaifu/sleepy-discord/edit/docs/source/link.html.md "Edit Documentation") [How To Link](https://yourwaifu.github.io/sleepy-discord/link.html)<br />
[:pencil:](https://github.com/yourWaifu/sleepy-discord/edit/docs/source/basic%20text%20bot.html.md "Edit Documentation") [How to make a Basic Text Bot](https://yourwaifu.github.io/sleepy-discord/basic%20text%20bot.html)<br />

If you like to edit them, [please check out the docs branch](https://github.com/yourWaifu/sleepy-discord/tree/docs) or click on the [:pencil:](https://github.com/yourWaifu/sleepy-discord/edit/docs/source/documentation.html.md "Edit Documentation").

# Why?
Just for the novelty of using a C++ library for Discord. I would also love for this thing to work on many things like consoles and maybe some microcontrollers.

# Example
```cpp
#include "sleepy_discord/websocketpp_websocket.h"

class MyClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override {
		if (message.startsWith("whcg hello"))
			sendMessage(message.channelID, "Hello " + message.author.username);
	}
};

int main() {
	MyClientClass client("token", 2);
	client.run();
}
```
Input: Message received
```
whcg hello
```
Possible Output: Message sent
```
Hello Sleepy Flower Girl
```

More complex examples:
 - [Rock Paper Scissors](https://github.com/yourWaifu/sleepy-discord/blob/master/examples/rock-paper-scissors/main.cpp)
 - [Unofficial Discord 3DS Client](https://github.com/yourWaifu/Unofficial-Discord-3DS-Client)
 - [WebAssembly Client](https://github.com/yourWaifu/sleepy-discord/tree/master/examples/wasm_example) (Work In Progress)
# Will Updating the library break my bot?

Yes, and for now I don't plan on making 0.0 versions backwards compatible with 1.0 versions or later.

# Requirements
Sleepy Discord doesn't require you to use any libraries, so that you can use any library you wish or your own code. However, Sleepy Discord provides native support for a few libraries and it is recommend you use those instead of writing your own.

* [OpenSSL](https://www.openssl.org/)
* [cpr](https://github.com/whoshuu/cpr)
* [Websocket++](https://github.com/zaphoyd/websocketpp)
or
[uWebSockets](https://github.com/uWebSockets/uWebSockets)

# Develop Branch
For the cutting edge of Sleepy Discord, check out the develop branch. Please use the develop branch with caution because it may not even be able to compile or it is 100% not tested at all. Other then that, the branch is used for code that may not work. Also check to see if it's not behind the master branch, unless you want to use an older version of Sleepy Discord.


# How to install and set up on linux, by someguyontheweb

do note that i am using Ubuntu 18.04.1 LTS Server, and running everything over ssh and samba

first step is to download and extract the repo files to anywhere on the linux system, then install the dependences (these may allready be installed by other packages).

where ever you extracted the repo (for reference this will be ./sleepy-discord/)
> mkdir ./sleepy-discord/Build/; cd ./sleepy-discord/Build; cmake ..; make;

make sure this does not cause any errors, if it does then you are missing dependencys or the extraction failed somewhat. once we have determined that no errors are caused, you can remove the Build directory
> rm -r Build/

so now that we can make sure this doesnt cause any errors, we will want to link this to our project, to do this we will create a symlink to the Project path (for reference this will be ./proj/) and then create a cmake file and a basic bash script to make things nice.
> ./proj/$ ln -s ./sleepy-discord/ ./proj/sleepy-discord

now make the CMakeLists.txt file that we will use to compile the project
> nano CMakeLists.txt

In File:
```cmake
 cmake_minimum_required (VERSION 2.6)
 project (ProjName)

 add_executable(ProjName main.cpp)
 include_directories(ProjName "${PROJECT_BINARY_DIR}")

 add_subdirectory("${PROJECT_SOURCE_DIR}/sleepy-discord")
 target_link_libraries(ProjName sleepy-discord)
```
now we dont want to clog up our source directory with cmake stuff, so lets make a small bash script to compile and run it in a sub directory,
> nano Build.sh; chmod +x Build.sh

In File:
```bash
#!/bin/bash
CURDIR=$(/bin/pwd)
# get script local, so we can execute it from anywere, WARNING this will break if the script is executed from a symlink
Root="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
ProjName="<ProjName>"

# if Build directory doesnt exist Make it
if [ ! -e $Root/Build ]; then
	mkdir $Root/Build
fi

# if the output file exists remove it (helps later in the script)
if [-e $Root/Build/$ProjName ]; then
	rm $Root/Build/$ProjName
fi

echo Compiling
cd $Root/Build
echo Executing Cmake
cmake ..
echo Executing Make
make
# go back to Source Dir, so that the Project will executing in the source enviroment
# you can change this to execute in an enviroment instead
cd ..

if [ -e $Root/Build/$ProjName ]; then
	echo "Compiled! Executing!"
	$Root/Build/$ProjName
else
	echo "Failed!! FIX errors!"
fi

# return to were the script was executed from
cd $CURDIR
echo Exited!
```

so now if you want to run your project just execute "./ProjName/Build.sh" and it will compleatly compile the project and run it for you, do note that compiling with cmake will take a while the first 2 times but after that its fairly quick. depending on how much you edited.

dont forget to get your hands on both libssl and libcurl, on ubuntu you can install them with
> sudo apt-get install libssl-dev libcurl4-openssl-dev


