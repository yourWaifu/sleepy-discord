cmake_minimum_required (VERSION 3.6)
project(sleepy-discord)

option(BUILD_EXAMPLES                "Build examples of Sleepy Discord and all dependencies"              OFF)
option(BUILD_SLEEPY_DISCORD_EXAMPLES "Build examples of Sleepy Discord"                                   OFF)
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

#set variables based on other variables
if(BUILD_EXAMPLES)
	set(BUILD_SLEEPY_DISCORD_EXAMPLES ON)
endif()

if(ENABLE_VOICE)
	set(USE_LIBOPUS ON)
	set(USE_LIBSODIUM ON)
endif()

if(USE_WEBSOCKETPP OR USE_UWEBSOCKETS)
	set(USE_ASIO ON)
endif()

# Find and Download libraries
if(AUTO_DOWNLOAD_LIBRARY)
	include(ExternalProject)
	include(buildtools/cmake/DownloadProject.cmake)
	if(USE_CPR)
		#to do install libssl-dev

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
			endif()
			#to do, set one for linux
		endif()
		if(NOT DEFINED USE_SYSTEM_CURL AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
			#using cmake to configure curl on linux has issues
			#so use system curl
			set(USE_SYSTEM_CURL ON CACHE BOOL "")
			set(USE_SYSTEM_CURL ON CACHE INTERNAL BOOL "")
		endif()

		download_project(
			PROJ           cpr
			GIT_REPOSITORY https://github.com/whoshuu/cpr.git
			GIT_TAG        master
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/cpr
			UPDATE_DISCONNECTED 1
		)

		add_subdirectory(deps/cpr)
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

		set(THREADS_PREFER_PTHREAD_FLAG ON)
		find_package(Threads REQUIRED)
		add_subdirectory(deps/websocketpp)
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
			GIT_REPOSITORY https://git.xiph.org/opus.git
			GIT_TAG        master
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/opus
			UPDATE_DISCONNECTED 1
		)
		add_subdirectory(deps/opus)
	endif()

	if(USE_LIBSODIUM)
		download_project(
			PROJ           libsodium
			GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
			GIT_TAG        stable
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/libsodium
			UPDATE_DISCONNECTED 1
		)
		#I would like use libsodium_BINARY_DIR as the BINARY_DIR but for some reason, that doesn't work
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
endif()

# Add Subdirectories
add_subdirectory(sleepy_discord)
if (BUILD_SLEEPY_DISCORD_EXAMPLES)
	add_subdirectory(examples/hello)
endif()