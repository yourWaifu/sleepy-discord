---
title: How To Link Sleepy Discord 

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---
[⮌ Go back to documentation](documentation.html)

#How To Link

<aside class="notice">
I'm going to assume that you have compiled Sleepy Discord or aquired a compiled binary
</aside>

#Visual Studio

##Preparing
Before we start, if are using a library that needs curl, for example cpr, then you need to compile it. If you used setup.py to download cpr, it should have also download curl for you in ``sleepy_discord\deps\cpr\opt\curl``.

> Here what the commands should look like for compiling curl

```shell
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
cd ..\..\sleepy-discord\deps\cpr\opt\curl\winbuild
nmake /f Makefile.vc mode=static VC=14
```

Here's a good guide to do just that, but before you read it remember to change things like ``Microsoft Visual Studio 12.0`` or ``VC=12`` or ``vc12`` to ``Microsoft Visual Studio 14.0`` or ``VC=14`` or ``vc14`` or whatever version of Visual Studio. [Anyway, here is the guide that I'm talking about.](https://stackoverflow.com/a/32168255) After you have compiled curl, place the include and lib folders into deps.

##Open Properties

**Step 1:** First you need a project, of course. Go make a new one if you haven't done so already.

**Step 2:** Right click on your project in the Solution Explorer and go to Properties.

![alt text](images/lopenproperties.png)

<aside class="notice">
Alt+F7 also works if you have only one project in your solution.
</aside>

##Include Directories

**Step 3:** On the left, go to VC++ Directories. Click on Include Directories and then click on the arrow on the right, and then click ``Edit...``

![alt text](images/lvcdir.png)

**Step 4:** Click on the white empty box at the top, and then click on the 3 dots on the right.

> The box of Include Directories should look something like this, but with the full path

```bash
$(projectdir)..\include
$(projectdir)..\deps
$(projectdir)..\deps\include
$(projectdir)..\include\sleepy_discord\IncludeNonexistent
```

**Step 5:** Select the folder containing the folder ``sleepy_discord``, and do the same for the dependencies(``deps``), include dependencies (``deps/include``), and the ``\include\sleepy_discord\IncludeNonexistent``. Also, it is important to note that the order of these folders matter. The compiler will look at the first path first, and and the 2nd one next, so ``\include\sleepy_discord\IncludeNonexistent`` needs to be last. This is so that we don't get "any include files not found" errors. Anyway, once done, click OK. These folders will be the folders that Visual C++ will look into to find header files for Sleepy_Discord.

##Library Directories

> The box of Library Directories should look like this, but with full paths

```bash
$(projectdir)$(Configuration)
$(projectdir)..\deps\lib
```

**Step 6:** Do the same thing for Library Directories, add the folder with Sleepy_Discord and all it's needed libraries. This tells Visual C++ to search for library files in these folders

##Add Dependencies

**Step 7:** To tell Visual C++ to link these library, click on the arrow to the left of Linker, and go to Input. Click on Additional Dependencies, and then click on the arrow on the right, and then click Edit...

![alt text](images/ladddeps.png)

> Here's a good example of the library files to list

```bash
sleepy_discord.lib
libcurl_a.lib
libcryptoMT.lib
libsslMT.lib
crypt32.lib
```

**Step 8:** On the text box at the top add ``sleepy_discord.lib`` then add all the other needed library files you also need. The librarys you will need to link to will depend on your deps/lib folder, system, platform, and configuration. Also, each library file is separated by a new line. Once you're done, remember to click OK.

##Preprocessor Definitions

<aside class="notice">
These steps are optional, and are here just in case you need them.
</aside>

**Step 9:** Click the arrow to the left of C/C++, and go to Preprocessor. Click on Preprocessor Definitions, and then click Edit...

**Step 10:** On the text box at the top add whatever preprocessor definitions, you need. You don't need any to link Sleepy Discord. This is here just in case, you need it.

##Build
Once that's all done, [move over to the last final step.](#now-its-time-to-compile)

#G++

```shell
cd ./examples/hello/
g++ -std=c++11 -I ../../include -I ../../deps -I ../../deps/include -I ../../include/sleepy_discord/IncludeNonexistent example0.cpp -L../../buildtools -L/usr/lib -lsleepy_discord -lcurl -lssl -lcrypto -lpthread -o example.out
```

This example command should compile and link the example code in examples/hello to Sleepy Discord with G++. Please note that this example assumes that libcurl.a, libssl.a, and libcrypto.a are located in /usr/lib.

**Preparing:** Before we start, we need the needed libraries. This depends on the libraries that you'll be using with Sleepy Discord. For example, to use CPR, you need the a compiled version of CPR (or maybe you could use CMake to that for you since that's what they recommend you to do, I guess) and libcurl. Another example is to use Websocket++, you'll need, you guessed it, the openSSL library files.

##Helpful Chart
To link the Sleepy Discord with g++, you'll need to add some options (Or whatever they are called). The helpful chart below should help breakdown what all the options in the example are for.

| Option         | Example                      | Reason |
|----------------|------------------------------|--------|
| ``-std=c++11`` | ``-std=c++11``               | Lets you compile c++11 code, you can change this to something higher then 11 if you like. |
| ``-I``         | ``-I/path/to/includes``      | Tells the compiler where to look for header files and other files to include. IMPORTANT NOTE: Order matters, make sure ``include/sleepy_discord/IncludeNonexistant`` is last. |
| ``-L``         | ``-L/path/to/library/files`` | Tells the compiler where to look for lib files |
|                | ``example.cpp``              | The files you want to compile. NOTE: these should before any ``-l`` |
| ``-l``         | ``-llib``                    | Tells the compiler which libraries to link with |
| ``-D``         | ``-DSomeVariable``           | Defines preprocessor variable |
| ``-o``         | ``-o example.out``           | The output file |

Once that's all done, [move over to the last final step.](#now-its-time-to-compile)

#Now, It's Time to Compile

> Here's some example code for you to compile

```cpp
/*If you are using uWebSockets,
  remember to change this to uwebsockets_websocket.h*/
#include "sleepy_discord/websocketpp_websocket.h"

class myClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) {
		if (message.startsWith("whcg hello"))
			sendMessage(message.channelID, "Hello " + message.author.username);
	}
};

int main() {
	myClientClass client("token", 2);
	client.run();
}
```

And now you should be able to build without any errors.

<aside class="notice">
Remember to change ``token``` to your token if you want to run the code.
</aside>

If you did get an error, then check that you followed the instructions correctly. And, if you are still getting errors look below, search issues or post an issue on the github repository.

#Common Issues

##‘SSL_R_SHORT_READ’ was not declared in this scope
```shell
error: ‘SSL_R_SHORT_READ’ was not declared in this scope
if (ERR_GET_REASON(ec.value()) == SSL_R_SHORT_READ) {
                                  ^~~~~~~~~~~~~~~~
```
[Check out issue #77](https://github.com/yourWaifu/sleepy-discord/issues/77)