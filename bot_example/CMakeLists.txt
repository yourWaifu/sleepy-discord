##### ##################### #####
##### BOT_EXAMPLE API TESTS #####
##### ##################### #####

# Initialize Bot
cmake_minimum_required(VERSION 3.6)

set(bot_example_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/example0.cpp
)

add_executable(bot_example ${bot_example_SOURCES})
target_link_libraries(bot_example sleepy_discord)