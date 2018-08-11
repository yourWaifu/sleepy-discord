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
	json.c
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

set_target_properties(sleepy-discord PROPERTIES CXX_STANDARD 11)
set_target_properties(sleepy-discord PROPERTIES CXX_STANDARD_REQUIRED ON)

target_compile_features(sleepy-discord PUBLIC cxx_std_11)

target_compile_options(sleepy-discord PUBLIC -Wall)

if(USE_CPR)
	target_include_directories(sleepy-discord PUBLIC ${CPR_INCLUDE_DIRS})
	target_link_libraries(sleepy-discord PUBLIC ${CPR_LIBRARIES})
endif()

if(USE_ASIO)
	target_include_directories(sleepy-discord PUBLIC ${asio_SOURCE_DIR}/asio/include)
endif()

if(USE_WEBSOCKETPP)
	target_include_directories(sleepy-discord PUBLIC ${websocketpp_SOURCE_DIR})
	target_link_libraries(sleepy-discord PUBLIC ssl crypto Threads::Threads)
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

target_include_directories(sleepy-discord
	PUBLIC
		${PROJECT_SOURCE_DIR}/include/sleepy_discord/IncludeNonexistent
)
