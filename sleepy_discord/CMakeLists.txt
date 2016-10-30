##### ############################## #####
##### SLEEPY-DISCORD API BUILDSCRIPT #####
##### ############################## #####

# Initialize API 
cmake_minimum_required(VERSION 3.6)

SET (sleepy_discord_SOURCES
    ./attachment.cpp
    ./attachment.h

    ./channel.cpp
    ./channel.h 

    ./client.cpp 
    ./client.h 

    ./common.cpp 
    ./common.h 

    ./default_functions.cpp 
    
    ./discord_object_interface.cpp 
    ./discord_object_interface.h 

    ./embed.cpp 
    ./embed.h 
    
    ./error.h 

    ./experimental.cpp 
    ./experimental.h 

    ./json.c 
    ./json.h 

    ./json_wrapper.cpp 

    ./message.cpp 
    ./message.h 

    ./sd_error.cpp 

    ./server.cpp 
    ./server.h 
    
    ./sleepy_discord.h 

    ./user.cpp 
    ./user.h 
)

add_library(sleepy_discord ${sleepy_discord_SOURCES})
target_include_directories(sleepy_discord INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)