
cmake_minimum_required (VERSION 3.6)
project (sleepydiscord)

# Not reccomended but I do it anyway
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/sleepy_discord/*.c*")

set(CCOMMON_FLAGS "-mtune=generic -O3 -Wall -c")

set(CMAKE_CXX_FLAGS_DEBUG "${CCOMMON_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g3 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CCOMMON_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CC_FLAGS_DEBUG "${CCOMMON_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g3 -ggdb")
set(CMAKE_CC_FLAGS_RELEASE "${CCOMMON_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")

include_directories("${CMAKE_SOURCE_DIR}/include/sleepy_discord")
include_directories("${CMAKE_SOURCE_DIR}/include/sleepy_discord/IncludeNonexistent")
message(STATUS "${INCLUDE_DIRECTORIES}")

# Open SSL
find_package(OpenSSL REQUIRED)

# why static >_>
add_library(sleepydiscord STATIC "${SOURCE_FILES}")

target_link_libraries(sleepydiscord "${OPENSSL_SSL_LIBRARY}")
