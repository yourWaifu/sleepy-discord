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
	experimental.cpp
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

target_compile_options(sleepy-discord PUBLIC -Wall)

if(USE_CPR)
	target_include_directories(sleepy-discord PUBLIC ${CPR_INCLUDE_DIRS})
	target_link_libraries(sleepy-discord PUBLIC ${CPR_LIBRARIES})
endif()

if(USE_WEBSOCKETPP)
	target_include_directories(sleepy-discord PUBLIC ${websocketpp_SOURCE_DIR})
	target_link_libraries(sleepy-discord PUBLIC ssl crypto Threads::Threads)
endif()

if(USE_ASIO)
	target_include_directories(sleepy-discord PUBLIC ${asio_SOURCE_DIR}/asio/include)
endif()

target_include_directories(sleepy-discord
	PUBLIC
		${PROJECT_SOURCE_DIR}/include/sleepy_discord/IncludeNonexistent
)