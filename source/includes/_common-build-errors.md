## Common Errors

### CA path only supported by ...

```console
CA path only supported by OpenSSL, GnuTLS or mbed TLS.  Set CURL_CA_PATH=none or enable one of those TLS backends.
```

libCurl couldn't set the CURL_CA_PATH variable as it's not supported for the TLS backend that will be used.

The easiest way to fix this is to install libCurl from a package manager. If that doesn't fix it, you'll need to enable a TLS backend for libCurl.

### No CMAKE_CXX_COMPILER ...

```console
No CMAKE_CXX_COMPILER could be found.

Tell CMake where to find the compiler by setting either the environment variable "CXX" or the CMake cache entry CMAKE_CXX_COMPILER to the full path to the compiler, or to the compiler name if it is in the PATH.
```

Usually this means you don't have a compiler installed. GCC, Clang, or MSVC are recommend. You can check if you have them installed by run the compiler's command in the terminal. If you have a compiler installed, set the environment variable ``CXX`` to the command for the C++ compiler or the path to the C++ compiler.

### sleepy-discord/CMakeLists.txt (add_subdirectory)

```console
CMake Error at dependencies/sleepy-discord/CMakeLists.txt:176 (add_subdirectory):
  The source directory

    /path/to/sleepy-discord/deps/lib

  does not contain a CMakeLists.txt file.
```

Usually this means, that something went wrong with the download for one of the library. The easiest solution is to delete the CMakeCache.txt in your build folder and running CMake again. If you want a different solution that would be to have CMake only download and configure that one lib, you can do this by open the CMakeCache.txt and removing the ``lib-name_BINARY_DIR:STATIC`` and ``lib-name_SOURCE_DIR:STATIC`` from the file. After that, save the file. Now run Cmake with ``AUTO_DOWNLOAD_LIBRARY:BOOL`` set to True, you can set this to False afterwards.

### unofficial-sodium

```console
  Could not find a package configuration file provided by "unofficial-sodium" with any of the following names:

    unofficial-sodiumConfig.cmake
    unofficial-sodium-config.cmake
```

This error only happens on windows as on Unix, make is used to compile libSodium but on windows things are a bit different. However, you can install libSodium using the unofficial sodium package from VCPKG.
