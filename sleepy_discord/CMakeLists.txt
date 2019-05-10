add_library(sleepy-discord
	asignments_client.cpp
	asio_udp.cpp
	attachment.cpp
	channel.cpp
	client.cpp
	cpr_session.cpp
	default_functions.cpp
	discord_object_interface.cpp
	embed.cpp
	endpoints.cpp
	gateway.cpp
	http.cpp
	invite.cpp
	json_wrapper.cpp
	message.cpp
	permissions.cpp
	sd_error.cpp
	server.cpp
	user.cpp
	uwebsockets_websocket.cpp
	voice.cpp
	voice_connection.cpp
	webhook.cpp
	websocketpp_websocket.cpp
)

target_include_directories(sleepy-discord
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/deps/include
	PRIVATE
		${PROJECT_SOURCE_DIR}/include/sleepy_discord
)

#The codebase is c++11 but both clang and visual c++ have issues with c++11
#These issues are fixed by using preprocessor defines to replace code that
#cause issues with c++14 code
set_target_properties(sleepy-discord PROPERTIES CXX_STANDARD 14)
set_target_properties(sleepy-discord PROPERTIES CXX_STANDARD_REQUIRED ON)
target_compile_features(sleepy-discord PUBLIC cxx_std_14)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(sleepy-discord PUBLIC -W3)
else()
	target_compile_options(sleepy-discord PUBLIC -Wall)
endif()

if (NOT ONLY_SLEEPY_DISCORD)
	#for some reason you a limited ammount of target_link_libraries calls
	set(LIBRARIES_TO_LINK)
	if(USE_CPR)
		if(CURL_STATICLIB)
			target_compile_definitions(${CPR_LIBRARIES} PUBLIC CURL_STATICLIB)
		endif()
		target_include_directories(sleepy-discord PUBLIC ${CPR_INCLUDE_DIRS})
		list(APPEND LIBRARIES_TO_LINK "${CPR_LIBRARIES}")
	endif()

	if(USE_ASIO)
		target_include_directories(sleepy-discord PUBLIC ${asio_SOURCE_DIR}/asio/include)
	endif()

	if(USE_WEBSOCKETPP)
		target_include_directories(sleepy-discord PUBLIC ${websocketpp_SOURCE_DIR})
		if (WIN32)
			find_package(OpenSSL REQUIRED)
			list(APPEND LIBRARIES_TO_LINK "OpenSSL::SSL" "OpenSSL::Crypto")
		elseif (UNIX)
			list(APPEND LIBRARIES_TO_LINK "ssl" "crypto" "Threads::Threads")
		endif()
	endif()

	if(USE_UWEBSOCKETS)
		find_library(LIB_UWS uWS
			PATHS ${uwebsockets_SOURCE_DIR}/lib
		)
		target_include_directories(sleepy-discord PUBLIC ${uwebsockets_SOURCE_DIR}/include)
		list(APPEND LIBRARIES_TO_LINK "${LIB_UWS}")
	endif()

	if(USE_LIBOPUS)
		target_include_directories(sleepy-discord PUBLIC ${opus_SOURCE_DIR}/include)
		list(APPEND LIBRARIES_TO_LINK "opus")
	endif()

	if(USE_LIBSODIUM)
		#NO_DEFAULT_PATH is used to force cmake to look at the path provided first
		find_library(LIB_SODIUM sodium
			PATHS ${libsodium_BINARY_DIR}/src/libsodium/.libs/
			NO_DEFAULT_PATH
		)
		find_library(LIB_SODIUM sodium)
		#for some reason, it keeps wanting to use libsodium_SOURCE_DIR instead of libsodium_BINARY_DIR
		#so replace libsodium_SOURCE_DIR when it's detected
		if(LIB_SODIUM MATCHES "${libsodium_SOURCE_DIR}")
			string(REPLACE "${libsodium_SOURCE_DIR}" "${libsodium_BINARY_DIR}" LIB_SODIUM ${LIB_SODIUM})
		endif()
		target_include_directories(sleepy-discord PUBLIC ${libsodium_SOURCE_DIR}/src/libsodium/include)
		list(APPEND LIBRARIES_TO_LINK "${LIB_SODIUM}")
	endif()
	target_link_libraries(sleepy-discord PUBLIC ${LIBRARIES_TO_LINK})
endif()

if(NOT Git_FOUND)
	target_compile_definitions(sleepy-discord PRIVATE NONEXISTANT_GIT_INFO)
endif()

if(ENABLE_VOICE)
	target_compile_definitions(sleepy-discord PRIVATE SLEEPY_VOICE_ENABLED)
endif()

target_include_directories(sleepy-discord
	PUBLIC
		${PROJECT_SOURCE_DIR}/include/sleepy_discord/IncludeNonexistent
)
