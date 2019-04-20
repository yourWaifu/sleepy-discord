add_library(sleepy-discord
	asignments_client.cpp
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
	json.c
	message.cpp
	permissions.cpp
	sd_error.cpp
	server.cpp
	user.cpp
	uwebsockets_websocket.cpp
	voice.cpp
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

target_compile_options(sleepy-discord PUBLIC -Wall)

if(USE_CPR)
	if(CURL_STATICLIB)
		target_compile_definitions(${CPR_LIBRARIES} PUBLIC CURL_STATICLIB)
	endif()
	target_include_directories(sleepy-discord PUBLIC ${CPR_INCLUDE_DIRS})
	target_link_libraries(sleepy-discord PUBLIC ${CPR_LIBRARIES})
endif()

if(USE_ASIO)
	target_include_directories(sleepy-discord PUBLIC ${asio_SOURCE_DIR}/asio/include)
endif()

if(USE_WEBSOCKETPP)
	target_include_directories(sleepy-discord PUBLIC ${websocketpp_SOURCE_DIR})
	if (WIN32)
		find_package(OpenSSL REQUIRED)
		target_link_libraries(sleepy-discord PUBLIC OpenSSL::SSL OpenSSL::Crypto)
	elseif (UNIX)
		target_link_libraries(sleepy-discord PUBLIC ssl crypto Threads::Threads)
	endif()
endif()

if(USE_UWEBSOCKETS)
	find_library(LIB_UWS uWS
		PATHS ${uwebsockets_SOURCE_DIR}/lib
	)
	target_include_directories(sleepy-discord PUBLIC ${uwebsockets_SOURCE_DIR}/include)
	target_link_libraries(sleepy-discord ${LIB_UWS})
endif()

if(USE_LIBOPUS)
	find_library(LIB_OPUS opus
		PATHS ${opus_SOURCE_DIR} ${opus_SOURCE_DIR}/.lib
	)
	target_include_directories(sleepy-discord PUBLIC ${opus_SOURCE_DIR}/include)
	target_link_libraries(sleepy-discord ${LIB_OPUS})
endif()

if(USE_LIBSODIUM)
	find_library(LIB_SODIUM sodium
		PATHS ${opus_SOURCE_DIR}/libsodium ${opus_SOURCE_DIR}/libsodium/.lib
	)
	target_include_directories(sleepy-discord PUBLIC ${opus_SOURCE_DIR}/include)
	target_link_libraries(sleepy-discord ${LIB_SODIUM})
endif()

if(NOT Git_FOUND)
	target_compile_definitions(sleepy-discord PRIVATE NONEXISTANT_GIT_INFO)
endif()

target_include_directories(sleepy-discord
	PUBLIC
		${PROJECT_SOURCE_DIR}/include/sleepy_discord/IncludeNonexistent
)
