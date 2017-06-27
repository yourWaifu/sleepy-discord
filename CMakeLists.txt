project(sleepy_discord)

cmake_minimum_required(VERSION 3.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# todo: add uwebsockets support
find_package(Threads REQUIRED)
find_package(websocketpp)
find_package(Boost COMPONENTS system)
find_package(asio)
find_package(cpr)
find_package(OpenSSL)

if(WEBSOCKETPP_FOUND)
    set(WEBSOCKET_IMPL "websocketpp")

    if(BOOST_NOT_FOUND AND ASIO_NOT_FOUND)
        message(FATAL_ERROR "Websocketpp requires asio to be present")
    endif()
else()
    set(WEBSOCKET_IMPL "uwebsockets")
endif()

mark_as_advanced(WEBSOCKET_IMPL)

add_subdirectory(sleepy_discord)
add_subdirectory(bot_example)
