cmake_minimum_required (VERSION 3.6)
project(sound-player)

if(NOT SLEEPY_DISCORD_CMAKE)
	set(ENABLE_VOICE ON CACHE BOOL "")
	add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/sleepy-discord)
endif()

add_executable(sound-player main.cpp)

if(NOT ENABLE_VOICE)
	message(FATAL_ERROR "ENABLE_VOICE needs to be set to on to be able to compile sound-player example.")
endif()

target_link_libraries(sound-player sleepy-discord)