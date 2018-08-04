cmake_minimum_required (VERSION 3.6)
project(sleepy-discord)

option(BUILD_EXAMPLES                "Build examples of Sleepy Discord and all dependances" OFF)
option(BUILD_SLEEPY_DISCORD_EXAMPLES "Build examples of Sleepy Discord"                     OFF)
option(USE_CPR                       "Use CPR http library"                                 ON )
option(USE_WEBSOCKETPP               "Use websocketpp websockets library"                   ON )
option(USE_UWEBSOCKETS               "Use uWebsockets websockets library"                   OFF)
option(USE_ASIO                      "Use ASIO network and I/O library (Used for UDP)"      OFF)
option(USE_LIBOPUS                   "Use Opus audio codec library"                         OFF)
option(USE_LIBSODIUM                 "Use libsodium cryptography library"                   OFF)

if(BUILD_EXAMPLES)
	set(BUILD_SLEEPY_DISCORD_EXAMPLES ON)
endif()

# Find and Download libraries
include(buildtools/cmake/DownloadProject.cmake)
if(USE_CPR)
	download_project(
		PROJ           cpr
		GIT_REPOSITORY https://github.com/whoshuu/cpr.git
		GIT_TAG        master
		SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/cpr
		UPDATE_DISCONNECTED 1
	)

	add_subdirectory(deps/cpr)
endif()

if(USE_WEBSOCKETPP)
	download_project(
		PROJ           websocketpp
		GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
		GIT_TAG        master
		SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/websocketpp
		UPDATE_DISCONNECTED 1
	)

	set(USE_ASIO ON)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	add_subdirectory(deps/websocketpp)
endif()

if(USE_ASIO)
	download_project(
		PROJ           asio
		GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
		GIT_TAG        master
		SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/asio
		UPDATE_DISCONNECTED 1
	)
endif()

# Add Subdirectories
add_subdirectory(sleepy_discord)
if (BUILD_EXAMPLES)
	add_subdirectory(examples/hello)
endif()