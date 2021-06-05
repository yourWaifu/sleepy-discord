add_library(sleepy-discord STATIC
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
	slash_commands.cpp
	user.cpp
	uwebsockets_websocket.cpp
	voice.cpp
	voice_connection.cpp
	webhook.cpp
	websocketpp_websocket.cpp
	zlib_compression.cpp
	stage_instance.cpp
)

target_include_directories(sleepy-discord
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/deps/include
	PRIVATE
		${PROJECT_SOURCE_DIR}/include/sleepy_discord
)

set(SLEEPY_INCLUDE_NONEXIST
	${PROJECT_SOURCE_DIR}/include/sleepy_discord/IncludeNonexistent)

#The codebase is c++11 but both clang and visual c++ have issues with c++11
#These issues are fixed by using preprocessor defines to replace code that
#cause issues with c++14 code
set_target_properties(sleepy-discord PROPERTIES CXX_STANDARD_REQUIRED ON)
target_compile_features(sleepy-discord PUBLIC cxx_std_11 cxx_return_type_deduction)

target_compile_definitions(sleepy-discord PUBLIC SLEEPY_DISCORD_CMAKE)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(sleepy-discord PUBLIC -W3)
else()
	target_compile_options(sleepy-discord PUBLIC -Wall)
endif()

set(LIB_CONFIG)
if (NOT ONLY_SLEEPY_DISCORD)
	#for some reason you a limited ammount of target_link_libraries calls
	set(LIBRARIES_TO_LINK)
	set(LIBRARIES_INCLUDE_DIRS)
	set(REQUIRED_PACKAGES)
	if(USE_CPR)
		if(CURL_STATICLIB)
			target_compile_definitions(${CPR_LIBRARIES} PUBLIC CURL_STATICLIB)
		endif()
		if(NOT SLEEPY_VCPKG)
			list(APPEND LIBRARIES_INCLUDE_DIRS "${CPR_INCLUDE_DIRS}")
		endif()
		list(APPEND LIBRARIES_TO_LINK "${CPR_LIBRARIES}")
		list(APPEND LIB_CONFIG "EXISTENT_CPR")
		list(APPEND REQUIRED_PACKAGES "cpr")
	else()
		list(APPEND LIB_CONFIG "NONEXISTENT_CPR")
	endif()

	if(USE_ASIO)
		if(SLEEPY_VCPKG)
			list(APPEND LIBRARIES_TO_LINK "asio asio::asio")
		else()
			list(APPEND LIBRARIES_INCLUDE_DIRS "${asio_SOURCE_DIR}/asio/include")
		endif()
		list(APPEND LIB_CONFIG "EXISTENT_ASIO")
		list(APPEND REQUIRED_PACKAGES "asio")
	elseif(USE_BOOST_ASIO)
		#Look for boost as an alt
		find_package(Boost REQUIRED)
		list(APPEND LIBRARIES_TO_LINK ${Boost_LIBRARIES})
		list(APPEND LIB_CONFIG "EXISTENT_BOOST_ASIO")
		list(APPEND LIB_CONFIG "EXISTENT_ASIO")
		list(APPEND REQUIRED_PACKAGES "Boost")
	else()
		list(APPEND LIB_CONFIG "NONEXISTENT_ASIO")
	endif()

	if(USE_WEBSOCKETPP)
		if(SLEEPY_VCPKG)
			list(APPEND LIBRARIES_TO_LINK "websocketpp::websocketpp")
			list(APPEND REQUIRED_PACKAGES "OpenSSL")
		else()
			list(APPEND LIBRARIES_INCLUDE_DIRS "${websocketpp_SOURCE_DIR}")
		endif()
		if (WIN32)
			find_package(OpenSSL REQUIRED)
			list(APPEND LIBRARIES_TO_LINK "OpenSSL::SSL" "OpenSSL::Crypto")
		elseif (UNIX)
			find_package(Threads REQUIRED)
			list(APPEND REQUIRED_PACKAGES "Threads")
			list(APPEND LIBRARIES_TO_LINK "ssl" "crypto" "Threads::Threads")
		endif()
		list(APPEND REQUIRED_PACKAGES "websocketpp")
		list(APPEND LIB_CONFIG "EXISTENT_WEBSOCKETPP")
	else()
		list(APPEND LIB_CONFIG "NONEXISTENT_WEBSOCKETPP")
	endif()

	if(USE_UWEBSOCKETS)
		find_library(LIB_UWS uWS
			PATHS ${uwebsockets_SOURCE_DIR}/lib
		)
		list(APPEND LIBRARIES_INCLUDE_DIRS "${UWEBSOCKETS_INCLUDE_DIRS}")
		list(APPEND LIBRARIES_TO_LINK "${LIB_UWS}")
		list(APPEND LIB_CONFIG "EXISTENT_UWEBSOCKETS")
	else()
		list(APPEND LIB_CONFIG "NONEXISTENT_UWEBSOCKETS")
	endif()

	if(USE_LIBOPUS)
		if(SLEEPY_VCPKG)
			list(APPEND REQUIRED_PACKAGES "Opus")
			list(APPEND LIBRARIES_TO_LINK "Opus::opus")
		else()
			list(APPEND LIBRARIES_INCLUDE_DIRS "${opus_SOURCE_DIR}/include")
			list(APPEND LIBRARIES_TO_LINK "opus")
		endif()
		list(APPEND LIB_CONFIG "EXISTENT_OPUS")
	else()
		list(APPEND LIB_CONFIG "NONEXISTENT_OPUS")
	endif()

	if(USE_LIBSODIUM)
		if (WIN32 OR SLEEPY_VCPKG)
			find_package(unofficial-sodium CONFIG REQUIRED)
			list(APPEND REQUIRED_PACKAGES "unofficial-sodium")
			list(APPEND LIBRARIES_TO_LINK "unofficial-sodium::sodium")
			find_path(SODIUM_INCLUDE_DIR NAMES sodium.h)
			list(APPEND LIBRARIES_INCLUDE_DIRS "${SODIUM_INCLUDE_DIR}")
		else()
			add_dependencies(sleepy-discord libsodium-make)
			ExternalProject_Get_property(libsodium-make BINARY_DIR)
			set(libsodium-make_BINARY_DIR ${BINARY_DIR})
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
		list(APPEND LIB_CONFIG "EXISTENT_SODIUM")
	else()
		list(APPEND LIB_CONFIG "NONEXISTENT_SODIUM")
	endif()

	if(USE_ZLIB_NG)
		get_target_property(ZLIB_BINARY_DIR zlib BINARY_DIR)
		list(APPEND LIBRARIES_INCLUDE_DIRS "${zlib-ng_SOURCE_DIR}/../" "${ZLIB_BINARY_DIR}")
		list(APPEND LIBRARIES_TO_LINK "zlib")
		list(APPEND LIB_CONFIG "EXISTENT_ZLIB_NG")
	else()
		list(APPEND LIB_CONFIG "NONEXISTENT_ZLIB_NG")
	endif()

	if(USE_ZLIB)
		list(APPEND LIBRARIES_TO_LINK "ZLIB::ZLIB")
		list(APPEND REQUIRED_PACKAGES "ZLIB")
		list(APPEND LIB_CONFIG "EXISTENT_ZLIB")
	else()
		list(APPEND LIB_CONFIG "NONEXISTENT_ZLIB")
	endif()

	target_link_libraries(sleepy-discord PUBLIC ${LIBRARIES_TO_LINK})
	target_include_directories(sleepy-discord PUBLIC ${LIBRARIES_INCLUDE_DIRS})
else()
	list(APPEND LIB_CONFIG
		"NONEXISTENT_CPR"
		"NONEXISTENT_ASIO"
		"NONEXISTENT_BOOST_ASIO"
		"NONEXISTENT_WEBSOCKETPP"
		"NONEXISTENT_UWEBSOCKETS"
		"NONEXISTENT_OPUS"
		"NONEXISTENT_SODIUM")
endif()

if(NOT Git_FOUND AND NOT SLEEPY_VCPKG)
	# Not needed for vcpkg
	target_compile_definitions(sleepy-discord PRIVATE NONEXISTANT_GIT_INFO)
endif()

if(ENABLE_VOICE)
	target_compile_definitions(sleepy-discord PUBLIC SLEEPY_VOICE_ENABLED)
	list(APPEND LIB_CONFIG "SLEEPY_VOICE_ENABLED")
endif()

if (SLEEPY_VCPKG)
	install(TARGETS sleepy-discord LIBRARY)
	install(DIRECTORY ../include/sleepy_discord TYPE INCLUDE CONFIGURATIONS Release)

	# strings for saving the configuration of the library 
	set(LIBRARIES_TO_LINK_STR "${LIBRARIES_TO_LINK}")
	set(LIBRARIES_INCLUDE_DIRS_STR "${LIBRARIES_INCLUDE_DIRS}")
	set(LIB_CONFIG_STR "${LIB_CONFIG}")
	set(REQUIRED_PACKAGES_STR "${REQUIRED_PACKAGES}")

	configure_file(../buildtools/cmake/sleepy-discord-config.cmake.in "sleepy-discord-config.cmake" @ONLY)
endif()

target_compile_definitions(sleepy-discord PUBLIC ${LIB_CONFIG})