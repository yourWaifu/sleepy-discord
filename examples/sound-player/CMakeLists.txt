cmake_minimum_required (VERSION 3.6)
project(sound-player)

add_executable(sound-player main.cpp)

set(ENABLE_VOICE ON CACHE BOOL "")
add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/sleepy-discord)
target_link_libraries(sound-player sleepy-discord)