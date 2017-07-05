---
title: How To Link Sleepy Discord 

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---
[< Go back to documentation](documentation.html)

#How To Link

<aside class="notice">
I'm going to assume that you have compiled Sleepy Discord
</aside>

##Visual Studio
<aside class="notice">
For now, Visual Studio is the only opinion.
</aside>
Step 1: First you need a project, of corse. Go make a new one if you haven't done so already.

Step 2: Right click on your project in the Solution Explorer and go to Properties.

<img src="images/lopenproperties.png" style='object-fit: contain'>

<aside class="notice">
Alt+F7 also works if you have only one project in your solution.
</aside>
Step 3: On the left, go to VC++ Directories. Click on Include Directories and then click on the arrow on the right, and then click ``Edit...``

<img src="images/lvcdir.png" style='object-fit: contain'>

Step 4: Click on the white empty box at the top, and then click on the 3 dots on the right.

> The box of Include Directories should look something like this, but with the full path

```bash
$(SolutionDir)
$(SolutionDir)deps
$(SolutionDir)deps\include
$(SolutionDir)sleepy_discord\IncludeNonexistent
```

Step 5: Select the folder containing the folder ``sleepy_discord``, and do the same thing for the dependencies(``deps``), include dependencies (``deps/include``), and the ``sleepy_discord/IncludeNonexistent``. Once done, click OK. These folders will be the folders that Visual C++ will look into to find header files for Sleepy_Discord.

> The box of Library Directories should look like this, but with full paths

```bash
$(SolutionDir)deps\lib
```

Step 6: Do the same thing for Library Directories, add the folder with Sleepy_Discord and all it's needed libraries. This tells Visual C++ to search for library files in these folders

Step 7: To tell Visual C++ to link these library, click on the arrow to the left of Linker, and go to Input. Click on Additional Dependencies, and then click on the arrow on the right, and then click Edit...

<img src="images/ladddeps.png" style='object-fit: contain'>

> Here's a good example of the library files to list

```bash
sleepy_discord.lib
libcurl_a.lib
ssleay32MT.lib
libeay32MT.lib
```

Step 8: On the text box at the top add ``sleepy_discord.lib`` then add all the other needed library files you also need. Also, each library file is separated by a new line. Once you're done, remember to click OK.

##Now, It's Time to Compile

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
			sendMessage(message.channel_id, "Hello " + message.author.username);
	}
};

int main() {
	myClientClass client("token", 2);
	client.run();
}
```

And now you should be able to build without any errors. If you did get an error, then check that you followed the instructions correctly. And, if you are still getting errors, post an issue on the github repository.