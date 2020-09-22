## Compile

Please click on the generator you used.

 * [Visual Studio](#visual-studio)
 * [Makefile](#makefile)
 * [Other](#other)

### Visual Studio

 1. In your build folder, open the sln file with Visual Studio or click ``Open Project`` in the CMake GUI.

 2. Open the Solution Explorer, right click on your project and click ``Build``.

<aside>
	You can also right click and click on ``Set up as Start up Project`` and then click on ``&#9658;Local Windows Debugger`` below the menu bar to run you project in debug mode.
</aside>

### Makefile

Open the command line or terminal, for example, CMD or Shell. Use ``cd`` to go to your build folder/directory and use the ``make`` command.

```shell
cd build
make
```

### Other

If other, go to your favorite search engine and look up ``How to compile using X`` and replace X with whatever generator you chose.

## Common Errors

### CA path only supported by ...

```
CA path only supported by OpenSSL, GnuTLS or mbed TLS.  Set CURL_CA_PATH=none or enable one of those TLS backends.
```

libCurl couldn't set the CURL_CA_PATH variable as it's not supported for the TLS backend that will be used.

The easiest way to fix this is to install libCurl from a package manager. If that doesn't fix it, you'll need to enable a TLS backend for libCurl.

### No CMAKE_CXX_COMPILER ...

```
No CMAKE_CXX_COMPILER could be found.

Tell CMake where to find the compiler by setting either the environment variable "CXX" or the CMake cache entry CMAKE_CXX_COMPILER to the full path to the compiler, or to the compiler name if it is in the PATH.
```

Usually this means you don't have a compiler installed. GCC, Clang, or MSVC are recommend. You can check if you have them installed by run the compiler's command in the terminal. If you have a compiler installed, set the environment variable ``CXX`` to the command for the C++ compiler or the path to the C++ compiler.

### sleepy-discord/CMakeLists.txt (add_subdirectory)

```
CMake Error at dependencies/sleepy-discord/CMakeLists.txt:176 (add_subdirectory):
  The source directory

    /path/to/sleepy-discord/deps/lib

  does not contain a CMakeLists.txt file.
```

Usually this means, that something went wrong with the download for one of the library. The easiest solution is to delete the CMakeCache.txt in your build folder and running CMake again. If you want a different solution that would be to have CMake only download and configure that one lib, you can do this by open the CMakeCache.txt and removing the ``lib-name_BINARY_DIR:STATIC`` and ``lib-name_SOURCE_DIR:STATIC`` from the file. After that, save the file. Now run Cmake with ``AUTO_DOWNLOAD_LIBRARY:BOOL`` set to True, you can set this to False afterwards.

### unofficial-sodium

```
  Could not find a package configuration file provided by "unofficial-sodium" with any of the following names:

    unofficial-sodiumConfig.cmake
    unofficial-sodium-config.cmake
```

This error only happens on windows as on Unix, make is used to compile libSodium but on windows things are a bit different. However, you can install libSodium using the unofficial sodium package from VCPKG.

