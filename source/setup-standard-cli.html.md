---
title: Using CMake Command Line Interface WIth Sleepy Discord

language_tabs:

toc_footers:

includes:
 - generator-compile
 - common-build-errors

search: true
---

# CMake Command Line Interface

Change current directory to the directory with your CMakeList.txt file.

```shell
cd your-project
```

Create a new directory named build.

```shell
mkdir build
```

Change current directory to the new build directory.

```shell
cd build
```

Choose a CMake Generator to use. [Here's a list of them](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html). In this example, we will use ``Visual Studio 15 2017 Win64``

Generate build configuration files using CMake on the previous directory with your chosen CMake Generator.

```shell
cmake .. -G "Visual Studio 15 2017" -A x64
```

