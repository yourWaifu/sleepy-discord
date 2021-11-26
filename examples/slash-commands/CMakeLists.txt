cmake_minimum_required (VERSION 3.6)
project(slash-commands)

add_executable(slash-commands main.cpp)

if(NOT SLEEPY_DISCORD_CMAKE)
	find_package(sleepy-discord)
endif()
target_link_libraries(slash-commands sleepy-discord)