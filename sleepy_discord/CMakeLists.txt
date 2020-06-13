add_library(sleepy-discord
	asignments_client.cpp
	asio_udp.cpp
	attachment.cpp
	channel.cpp
	client.cpp
	cpr_session.cpp
	default_functions.cpp
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
if (CXX_STANDARD LESS 14 OR NOT CXX_STANDARD)
	set_target_properties(sleepy-discord PROPERTIES CXX_STANDARD 14)
	set_target_properties(sleepy-discord PROPERTIES CXX_STANDARD_REQUIRED ON)
	target_compile_features(sleepy-discord PUBLIC cxx_std_14)
endif()
target_compile_definitions(sleepy-discord PUBLIC SLEEPY_DISCORD_CMAKE)

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
		target_compile_definitions(sleepy-discord PUBLIC EXISTENT_CPR)
	else()
		target_compile_definitions(sleepy-discord PUBLIC NONEXISTENT_CPR)
	endif()

	if(USE_ASIO)
		target_include_directories(sleepy-discord PUBLIC ${asio_SOURCE_DIR}/asio/include)
		target_compile_definitions(sleepy-discord PUBLIC EXISTENT_ASIO)
	else()
		target_compile_definitions(sleepy-discord PUBLIC NONEXISTENT_ASIO)
		#Look for boost as an alt
		find_package(Boost)
		if(Boost_FOUND)
			list(APPEND LIBRARIES_TO_LINK ${Boost_INCLUDE_DIRS})
			target_compile_definitions(sleepy-discord PUBLIC EXISTENT_BOOST_ASIO)
		else()
			target_compile_definitions(sleepy-discord PUBLIC NONEXISTENT_BOOST_ASIO)
		endif()
	endif()

	if(USE_WEBSOCKETPP)
		target_include_directories(sleepy-discord PUBLIC ${websocketpp_SOURCE_DIR})
		if (WIN32)
			find_package(OpenSSL REQUIRED)
			list(APPEND LIBRARIES_TO_LINK "OpenSSL::SSL" "OpenSSL::Crypto")
		elseif (UNIX)
			list(APPEND LIBRARIES_TO_LINK "ssl" "crypto" "Threads::Threads")
		endif()
		target_compile_definitions(sleepy-discord PUBLIC EXISTENT_WEBSOCKETPP)
	else()
		target_compile_definitions(sleepy-discord PUBLIC NONEXISTENT_WEBSOCKETPP)
	endif()

	if(USE_UWEBSOCKETS)
		find_library(LIB_UWS uWS
			PATHS ${uwebsockets_SOURCE_DIR}/lib
		)
		target_include_directories(sleepy-discord PUBLIC ${uwebsockets_SOURCE_DIR}/include)
		list(APPEND LIBRARIES_TO_LINK "${LIB_UWS}")
		target_compile_definitions(sleepy-discord PUBLIC EXISTENT_UWEBSOCKETS)
	else()
		target_compile_definitions(sleepy-discord PUBLIC NONEXISTENT_UWEBSOCKETS)
	endif()

	if(USE_LIBOPUS)
		target_include_directories(sleepy-discord PUBLIC ${opus_SOURCE_DIR}/include)
		list(APPEND LIBRARIES_TO_LINK "opus")
		target_compile_definitions(sleepy-discord PUBLIC EXISTENT_OPUS)
	else()
		target_compile_definitions(sleepy-discord PUBLIC NONEXISTENT_OPUS)
	endif()

	if(USE_LIBSODIUM)
		if (WIN32)
			find_package(unofficial-sodium CONFIG REQUIRED)
			list(APPEND LIBRARIES_TO_LINK "unofficial-sodium::sodium")
			find_path(SODIUM_INCLUDE_DIR NAMES sodium.h)
			target_include_directories(sleepy-discord PUBLIC ${SODIUM_INCLUDE_DIR})
		else()
			add_dependencies(sleepy-discord libsodium-make)
			ExternalProject_Get_property(libsodium-make BINARY_DIR)
			set(libsodium-make_BINARY_DIR ${BINARY_DIR})
			message(WARNING ${libsodium-make_BINARY_DIR})
			set(
				LIB_SODIUM_BUILDS 
				${libsodium-make_BINARY_DIR}/src/libsodium/.libs/${CMAKE_STATIC_LIBRARY_PREFIX}sodium${CMAKE_STATIC_LIBRARY_SUFFIX}
			)
			target_include_directories(
				sleepy-discord PUBLIC
				${libsodium_SOURCE_DIR}/src/libsodium/include
				${libsodium-make_BINARY_DIR}/src/libsodium/include
			)
			list(APPEND LIBRARIES_TO_LINK "${LIB_SODIUM_BUILDS}")
		endif()
		target_compile_definitions(sleepy-discord PUBLIC EXISTENT_SODIUM)
	else()
		target_compile_definitions(sleepy-discord PUBLIC NONEXISTENT_SODIUM)
	endif()
	target_link_libraries(sleepy-discord PUBLIC ${LIBRARIES_TO_LINK})
endif()

if(NOT Git_FOUND)
	target_compile_definitions(sleepy-discord PRIVATE NONEXISTANT_GIT_INFO)

endif()

if(ENABLE_VOICE)
	target_compile_definitions(sleepy-discord PUBLIC SLEEPY_VOICE_ENABLED)
endif()