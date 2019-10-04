---
title: Standard Configuration

language_tabs:

toc_footers:

includes:

search: true
---

# Standard Configuration

## What You Need

 * [CMake](https://cmake.org/download/)
 * OpenSSL library
   * For Windows users, you can [Download and use the installer (Don't download the light version or you'll run into errors)](https://slproweb.com/products/Win32OpenSSL.html) or any other binaries [found here](https://wiki.openssl.org/index.php/Binaries) or [via a simple google search](https://www.google.com/search?q=openssl+windows+precompiled).
   * For Linux users, use your package manager and install libssl-dev or openssl-devel. The name depends on your package manager.
 * A C++ Compiler
   * [Go here for build status](https://github.com/yourWaifu/sleepy-discord#build-status).
   * For Visual Studio users, Visual Studio has a c++ compiler.

## Instructions

### Creating a CMakeList file

```cmake
#CMakeLists.txt
cmake_minimum_required (VERSION 3.6)
project(example)

add_executable(replace-me source.cpp)
```

 1. In case you haven't already created a CMakeLists.txt file for your project. please do so on the root directory or folder of your project.

 2. Replace replace-me with the name of your project or executable.

## Linking Sleepy Discord

```cmake
#Add these two lines
add_subdirectory(path/to/sleepy-discord)
target_link_libraries(replace-me sleepy-discord)
```

 3. Add these two lines to the CMakeList.txt so that Sleepy Discord will be linked.

 4. Replace path/to/sleepy-discord with the [relative path](https://en.wikipedia.org/wiki/Path_(computing)#Absolute_and_relative_paths) to the sleepy-discord directory or folder. For example, if your CMakeList is in ``project/`` and sleepy-discord is in ``project/externals/``, then the relative path is ``externals/sleepy-discord``. Windows users, remember to use ``/``. Also remember to replace replace-me with the name of your executable.

<aside class="note">
In ``add_subdirectory``, using a directory that isn't a subdirectory to the current folder will require you to add a path to the binary folder for sleepy-discord as the 2nd parameter. It is recommended that you don't do this.
</aside>

## Example

```cmake
cmake_minimum_required (VERSION 3.6)
project(light-blob)

add_executable(light-blob light.cpp blob.cpp
main.cpp ui/switch.cpp
material/metal.cpp material/plastic.cpp material/glass.cpp)

add_subdirectory(deps/sleepy-discord)
target_link_libraries(light-blob sleepy-discord)
```

## Next Step

Using CMake to Generate build configuration files.

 * [CMake App (GUI)](setup-standard-gui)
 * [CMake Command (CLI)](setup-standard-cli)
 * [Visual Studio CMake](setup-standard-vs-cmake)