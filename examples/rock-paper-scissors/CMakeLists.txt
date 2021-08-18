cmake_minimum_required (VERSION 3.6)
project(rock-paper-scissors)

add_executable(rock-paper-scissors main.cpp)

add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/sleepy-discord)
target_link_libraries(rock-paper-scissors sleepy-discord)