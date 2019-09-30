---
title: How To Compile Sleepy Discord 

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---
[⮌ Go back to documentation](documentation.html)

#How to Compile
<aside class="warning">
Unless you need to compile the library yourself, it's recommended you use <a href="setup-standard">the standard configuration</a> which automates most of those steps.
</aside>

##Downloading Sleepy Discord

[Just click here to download.](https://github.com/NoNamer64/sleepy-discord/zipball/master)

Then unzip it by extracting the files out of the zip file.

##Preparing
It's important to understand the requirements needed to run this library. You can use this library without any dependencies, but it's easier with dependencies. To help you decide on the dependencies to use with this library, here are a list of options and other important details you will need to know when making your choice.

###Features
|Feature|Example|Requirements|
|-------|-----------|------------|
|REST API|Send a message using a request|Any HTTP Secure library|
|Gateway|Receiving a message at any time|Any Websockets Secure library|
|Voice|Talk in a voice channel|Any Websockets Secure library, Any UDP library, libOpus, and libsodium|

###Libraries/Options
|Option|Description|Dependencies|
|------|-----------|------------|
|Websocket++|A websocket library|OpenSSL and ASIO|
|uWebsockets|A websocket library|OpenSSL, zlib, and libuv or ASIO (both optional on Linux)|
|Custom Websockets|Define how Websockets will handled|None|
|CPR|A HTTP library|libCURL|
|Custom Session Class|Define how HTTP request will be handled|None|
|ASIO|Networking library, used for UDP|None|
|Custom UDP|Define how UDP request will be handled|None|
|libOpus|A audio encoder and decoder library|None|
|libsodium|An encryption library|None|
<aside>
For people developing things for platforms outside of the main 3 operating systems, please make sure to use libraries are compatible with what you are developing for. You may use whatever you like thanks to custom.
</aside>
[](TODO add links to all and how to for the custom stuff)

Can't decide? We recommend Websocket++, and CPR.

#Getting Setup

There's few options for getting setup to compile

* [CMake (Recommended)](#cmake)
* [Python Setup Scirpt](#python-setup-script)
* [Manual (last resort)](#manual-setup)

Can't decide? We recommend CMake.

##CMake

### List of Options
|Option|Variable|Default value|Requires OpenSSL|
|------|--------|-------------|----------------|
|Websocket++|USE_WEBSOCKETPP|ON|YES|
|uWebsockets|USE_UWEBSOCKETS|OFF|YES|
|CPR|USE_CPR|ON|NO|YES|
|ASIO|USE_ASIO|OFF but ON when USE_WEBSOCKETPP is ON|NO|
|libOpus|USE_LIBOPUS|OFF|NO|
|libsodium|USE_LIBSODIUM|OFF|NO|

### Installing OpenSSL

Take a look at the chart above, that'll tell you if you need OpenSSL. If you already have OpenSSL or don't need it, skip to [Generating Build Configuration Files](#generating-build-configuration-files).

Options (Choose one option):

* For Windows Users:
  * [VCPKG](#vcpkg)
  * [Windows Installer](#windows-installer)
* For Linux / Bash Users:
  * [Linux](#linux)

#### VCPKG

> In Windows, run these commands

```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
.\vcpkg install openssl-windows
```

It is recommend that you download openSSL using a package manager like [vcpkg](https://github.com/Microsoft/vcpkg). Also in case, you don't have git, make sure you download it from [git's website](https://git-scm.com/download/win).

When running CMake, you'll need to set some values for this to work. After installing vcpkg integration, vcpkg will print ``CMake projects should use: "X"``. Copy what's in between the ``""`` and paste it when using the cmake command. The right or below should have examples on how to do this, remember to replace ``D:\path\to\vcpkg`` with the path to vcpkg.

```shell
cmake .. -DBUILD_SLEEPY_DISCORD_EXAMPLES=1 -DCMAKE_TOOLCHAIN_FILE=D:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake -DOPENSSL_USE_STATIC_LIBS=TRUE -DOPENSSL_ROOT_DIR="D:\path\to\vcpkg\installed\x86-windows-static" -DBUILD_CPR_TESTS=OFF
```

#### Windows Installer

[Go here](https://slproweb.com/products/Win32OpenSSL.html), download the installer (Don't download the light version), and follow the on screen instructions.

#### Linux 

```shell
sudo apt-get install libssl-dev
```

It's that easy. Well in case, cmake still can't find openssl, then you might need to define the variables ``OPENSSL_USE_STATIC_LIBS``, ``OPENSSL_ROOT_DIR``.

### Generating Build Configuration Files

Options (Choose one option):

* For Visual Studio Users:
  * [Generating Visual Studio Solution](#generating-visual-studio-solution)
* For Linux / Bash Users:
  * [Generating Makefile](#generating-makefile)

#### Generating Makefile

```powershell
mkdir build
cd build
cmake ..
make
```

To use CMake, you'll obviously need CMake. You can download it from [their website](https://cmake.org/download/) or from your package manager. You will also need openSSL, this can be simply fixed by downloading libssl-dev from your package manager or download from [any of the urls listed here](https://wiki.openssl.org/index.php/Binaries). If you are on linux, just run the commands in the example. After that's done, [Go to the next step.](#the-next-step)

#### Generating Visual Studio Solution

> Replace <\path\to\openSSL> with the path to the OpenSSL folder and replace <\path\to\vcpkg> with path to vcpkg folder.

```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 15 2017" -A x64 -DCMAKE_TOOLCHAIN_FILE=<\path\to\vcpkg>\scripts\buildsystems\vcpkg.cmake -DOPENSSL_ROOT_DIR=<\path\to\openSSL>
```

<\path\to\openSSL\> can be find somewhere in <\path\to\vcpkg>\installed\ for vcpkg and C:\ for Windows Installer. You can also use ``-DOPENSSL_USE_STATIC_LIBS=TRUE`` to use the static library. Once the command has finished it's task, go to the build folder and open the sleepy-discord.sln file that was just created. Open solution explorer, right click on sleepy-discord and click on build. After that's done, [Go to the next step.](#the-next-step)

<aside>If you want fancy, you can open the cmake list files in Visual Studio. <a href = "https://docs.microsoft.com/en-us/cpp/ide/cmake-tools-for-visual-cpp?view=vs-2017">You'll need to read this tho.</a></aside>

##Python setup script
```powershell
PS C:\> python -v
Python 3.6.3 (v3.6.3:2c5fed8, Oct  3 2017, 17:26:49) [MSC v.1900 32 bit (Intel)] on win32
>>> quit()
```
Make sure you have Python 3.6.3, by using ``python -V``.

```powershell
PS C:\sleepy-discord-master\> .\setup.py
What OS are you building Sleepy Discord for?
0 - Windows
1 - Linux
2 - macOS
> 0
What libraries do you want to use?
0 - CPR
1 - Websocket++
2 - uWebSockets
> 0 1
```
Run the script ``setup.py`` by double clicking the py file, or call ``.\setup.py`` in the command line. Then just follow the instructions on screen. Once that's done go to [Compiling Sleepy Discord](#compiling-sleepy-discord).

##Manual setup
###Downloading CPR
<aside class="notice">
You may skip this step, if you are planning on using your own http library
</aside>
The CPR library can be found here [https://github.com/whoshuu/cpr](https://github.com/whoshuu/cpr). Once downloaded, in the sleepy-discord folder, make a new folder called deps (short for dependencies). Inside the deps, place the cpr-master folder in there, and rename it to cpr. Open the cpr folder, and take the include folder, and place it inside deps.
<pre>
![alt text](images/cdeps.png)
![alt text](images/cindeps.png)
</pre>

And in the cpr folder, open the opt folder. You should see an empty folder named curl. You will need to place curl there. Go here [https://github.com/whoshuu/cpr/tree/master/opt](https://github.com/whoshuu/cpr/tree/master/opt) and click on curl and download curl. Open the zip file and curl folder, and then drag everything in that folder into the curl folder that's inside the opt folder. And just build curl. If on windows, just open the buildconf.bat file.
<pre>
![alt text](images/cincurl.png)
</pre>

###Downloading the Websocket Library
<aside class="notice">
You may skip this step, if you are planning on using other websocket libraries
</aside>

* [Websocket++](#websocketpp)
* [uWebSockets](#uwebsockets)

###Websocketpp
You can find the Websocket++ library here [https://github.com/zaphoyd/websocketpp](https://github.com/zaphoyd/websocketpp). Once downloaded, open the zip file, and the websocketpp-master folder, and extract the websocketpp folder to the deps\include folder.
<pre>
![alt text](images/cindeps2.png)
</pre>

You are also going to need the Asio library, that can be found here [http://think-async.com/Asio/Download](http://think-async.com/Asio/Download). Open the zip file, and asio folder, and include folder. Extract the asio folder, and asio.hpp file to your deps\include folder.
<aside class="notice">
Sleepy Discord uses version 1.10.8, and does not work with 1.10.6. [The latest version can be downloaded here](https://sourceforge.net/projects/asio/files/latest/download)
</aside>

Last, you are also going to need openSSL. There's two options:

1. Download precompiled openSSL: [wiki.openssl.org](https://wiki.openssl.org/index.php/Binaries) or [npcglib.org](https://www.npcglib.org/~stathis/blog/precompiled-openssl/). You will be needing 7-zip, WinRAR or other program which one is able to unzip .7z files.

2. Or download it from [https://www.openssl.org/](https://www.openssl.org/). Once downloaded, extract the folder inside to somewhere to work on it. To install look at the instructions in INSTALL because this has its own set of instructions including downloading and installing some other dependencies. 

Once one of those two options are done, move all the .h files in openssl\include\openssl into deps\include\openssl. You'll also need to place lib folder into deps, you'll need this to link Sleepy Discord. 
<pre>
![alt text](images/cininclude.png)
</pre>

#Compiling Sleepy Discord
<u>
There's only two options:
</u>

* [Make](#make)
* [Visual Studio](#visual-studio)

##Make
```shell
cd buildtools/
make -f Makefile.linux
```
Call those commands in your shell, and you're done. It should compile a file called libsleepy_discord.a. After that, [go to the next step.](#the-next-step)

##Visual Studio
Open the sleepy_discord folder and find the sleepy_discord.vcxproj file. Open sleepy_discord.vcxproj with Visual Studio, of course. At the top, there a toolbar, click build and then build sleepy_discord. And you've compiled Sleepy Discord. [Go to the next step.](#the-next-step)

<aside>
For now, compiling for x64 doesn't work just yet.
</aside>
<aside class="success">
If you were able to compile the library, but can't find it, look up at the toolbar, click Project and then Properties.<br>
Go to Configuration Properties -> General -> Output Directory<br>
click on Output Directory, and then click on the drop down arrow and click edit. In the 2nd textbox it'll tell you were it compile it to.
Then do the same thing for the Target Name to get the name of the file.
</aside>
<aside class="success">
Don't forget to change the configuration to ``Release`` if you don't want the debug version.
</aside>

##Common Issues

If you don't see your issue listed here, check [the list of issues on the Github repo](https://github.com/yourWaifu/sleepy-discord/issues) and if your issue isn't listed in there, report it by creating a new issue.

### cannot open input file 'libcurl_a.lib'

It looks like you don't have libcurl_a.lib. This guide only touches on compiling Sleepy Discord. However, you'll need it once we start linking Sleepy Discord to your code. So, [go to the next step,](link.html) which should be about linking.

##The Next Step
Now that you have a compiled Sleepy Discord, head over to [How to Link](link.html) to link it to a new or existing project.
