cmake_minimum_required (VERSION 3.6)
project(sleepy-discord)

option(BUILD_EXAMPLES                "Build examples of Sleepy Discord and all dependencies"              OFF)
option(SLEEPY_DISCORD_BUILD_EXAMPLES "Build examples of Sleepy Discord"                                   OFF)
option(ONLY_SLEEPY_DISCORD           "Sleepy Discord but none of the dependencies, except build in onces" OFF)
option(ENABLE_VOICE                  "Enable voice support"                                               OFF)
if (NOT ONLY_SLEEPY_DISCORD)
	option(AUTO_DOWNLOAD_LIBRARY         "Automatically download sleepy discord standard config dependencies" ON )
	option(USE_CPR                       "Use CPR http library"                                               ON )
	option(USE_WEBSOCKETPP               "Use websocketpp websockets library"                                 ON )
	option(USE_UWEBSOCKETS               "Use uWebsockets websockets library"                                 OFF)
	option(USE_ASIO                      "Use ASIO network and I/O library (Used for UDP)"                    OFF)
	option(USE_LIBOPUS                   "Use Opus audio codec library"                                       OFF)
	option(USE_LIBSODIUM                 "Use libsodium cryptography library"                                 OFF)
endif()

#Define a variable to use to check if this file has been executed
set(SLEEPY_DISCORD_CMAKE ON)

#set variables based on other variables
if(BUILD_EXAMPLES)
	set(SLEEPY_DISCORD_BUILD_EXAMPLES ON)
endif()

if(ENABLE_VOICE)
	set(USE_LIBOPUS ON)
	set(USE_LIBSODIUM ON)
endif()

if(USE_WEBSOCKETPP OR USE_UWEBSOCKETS)
	set(USE_ASIO ON)
endif()

find_package(Git)

# Find and Download libraries
if(AUTO_DOWNLOAD_LIBRARY)

	if(NOT Git_FOUND)
		string(CONCAT GIT_NOT_FOUND_WARNING
			"GIT was not found, CMake list will likely fail. "
			"Please make sure that GIT is inside your PATH variable")
		message(WARNING ${GIT_NOT_FOUND_WARNING})
	endif()

	#to do find librarys before downloading them

	include(ExternalProject)
	include(buildtools/cmake/DownloadProject.cmake)
	if(USE_CPR)
		#to do install libssl-dev
		download_project(
			PROJ           cpr
			GIT_REPOSITORY https://github.com/whoshuu/cpr.git
			GIT_TAG        b748095
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/cpr
			UPDATE_DISCONNECTED 1
		)
	endif()

	if(USE_ASIO)
		# Note websocketpp doesn't work with the latest version of asio
		# Remember to go back to the master version after they are fixed
		download_project(
			PROJ           asio
			GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
			GIT_TAG        22afb86
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/asio
			UPDATE_DISCONNECTED 1
		)
	endif()

	if(USE_WEBSOCKETPP)
		download_project(
			PROJ           websocketpp
			GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
			GIT_TAG        master
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/websocketpp
			UPDATE_DISCONNECTED 1
		)
	endif()

	if(USE_UWEBSOCKETS)
		#to do install zlib
		download_project(
			PROJ           uwebsockets
			GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
			GIT_TAG        master
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/uwebsockets
			UPDATE_DISCONNECTED 1
			BUILD_COMMAND  ${MAKE}
		)
	endif()

	if(USE_LIBOPUS)
		#needs libtool
		download_project(
			PROJ           opus
			GIT_REPOSITORY https://gitlab.xiph.org/xiph/opus.git
			GIT_TAG        7628d844
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/opus
			UPDATE_DISCONNECTED 1
		)
	endif()

	if(USE_LIBSODIUM AND NOT WIN32)
		download_project(
			PROJ           libsodium
			GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
			GIT_TAG        stable
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/libsodium
			UPDATE_DISCONNECTED 1
		)
		ExternalProject_Add(libsodium-make
			SOURCE_DIR          "${libsodium_SOURCE_DIR}"
			BINARY_DIR          "${libsodium_BINARY_DIR}"
			PATCH_COMMAND       "${libsodium_SOURCE_DIR}/autogen.sh"
			CONFIGURE_COMMAND   "${libsodium_SOURCE_DIR}/configure"
			BUILD_COMMAND       "make"
			INSTALL_COMMAND     "cp" "-TR" "${libsodium_BINARY_DIR}/src/libsodium/include/" "${libsodium_SOURCE_DIR}/src/libsodium/include/"
			TEST_COMMAND        ""
		)
	endif()
else()
	if(USE_CPR)
		set(cpr_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/cpr)
	endif()
	if(USE_ASIO)
		set(asio_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/asio)
	endif()
	if(USE_WEBSOCKETPP)
		set(websocketpp_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/websocketpp)
	endif()
	if(USE_UWEBSOCKETS)
		set(uwebsockets_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/uwebsockets)
	endif()
	if(USE_LIBOPUS)
		set(opus_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/opus)
	endif()
	if(USE_LIBSODIUM AND NOT WIN32)
		set(libsodium_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/libsodium)
	endif()
endif()

# Configure libraries
if(USE_CPR)
	#the cmake test has issues on visual studio so disable them
	if(NOT DEFINED BUILD_CPR_TESTS)
		set(BUILD_CPR_TESTS OFF CACHE BOOL "")
	endif()

	set(BUILD_CURL_EXE OFF CACHE BOOL "")
	set(CURL_STATICLIB ON CACHE BOOL "")
	set(CURL_ZLIB OFF CACHE BOOL "")

	#set what ssl library to use for curl based on OS
	#this fixes
	if(NOT DEFINED CMAKE_USE_OPENSSL)
		if(WIN32)
			set(CMAKE_USE_WINSSL ON CACHE BOOL "")
			set(CMAKE_USE_OPENSSL OFF CACHE BOOL "")
		elseif(APPLE)
			set(CMAKE_USE_DARWINSSL ON CACHE BOOL "")
			set(CMAKE_USE_OPENSSL OFF CACHE BOOL "")
		elseif(UNIX)
			set(CMAKE_USE_OPENSSL OFF CACHE BOOL "")
		endif()
	endif()
	if(NOT DEFINED USE_SYSTEM_CURL AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		#using cmake to configure curl on linux has issues
		#so use system curl
		set(USE_SYSTEM_CURL ON CACHE BOOL "")
	endif()

	add_subdirectory(deps/cpr)
endif()

if(USE_WEBSOCKETPP)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	add_subdirectory(deps/websocketpp)
endif()

if(USE_LIBOPUS)
	add_subdirectory(deps/opus)
endif()

# Get Version Info
if(Git_FOUND)
	execute_process(
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
		COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
		OUTPUT_VARIABLE SLEEPY_DISCORD_VERSION_BUILD
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
		COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
		OUTPUT_VARIABLE SLEEPY_DISCORD_VERSION_BRANCH
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
		COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
		OUTPUT_VARIABLE SLEEPY_DISCORD_VERSION_HASH
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if (SLEEPY_DISCORD_VERSION_BRANCH STREQUAL "master")
		set(SLEEPY_DISCORD_VERSION_IS_MASTER "1")
	else()
		set(SLEEPY_DISCORD_VERSION_IS_MASTER "0")
	endif()
	execute_process(
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
		COMMAND ${GIT_EXECUTABLE} describe --always --long --dirty
		OUTPUT_VARIABLE SLEEPY_DISCORD_VERSION_DESCRIPTION
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if(
		NOT SLEEPY_DISCORD_VERSION_DESCRIPTION STREQUAL ""
	)
		set(SLEEPY_DISCORD_VERSION_DESCRIPTION_CONCAT " ")
	else()
		set(SLEEPY_DISCORD_VERSION_DESCRIPTION_CONCAT "")
	endif()
	configure_file(
		"${PROJECT_SOURCE_DIR}/include/sleepy_discord/version.h.in"
		"${PROJECT_SOURCE_DIR}/include/sleepy_discord/version.h"
	)
endif()

# Add Subdirectories
add_subdirectory(sleepy_discord)
if (SLEEPY_DISCORD_BUILD_EXAMPLES)
	add_subdirectory(examples/hello)
	if (ENABLE_VOICE)
		add_subdirectory(examples/sound-player)
	endif()
endif()