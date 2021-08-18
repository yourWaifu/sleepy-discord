cmake_minimum_required (VERSION 3.6)
project(hello-example)

if(NOT SLEEPY_DISCORD_CMAKE)
	add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/sleepy-discord)
endif()

add_executable(hello-example
	example0.cpp
)

target_link_libraries(hello-example
	sleepy-discord
)