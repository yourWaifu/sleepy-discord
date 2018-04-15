
cmake_minimum_required (VERSION 3.6)
project(sleepy-discord)

set(CCOMMON_FLAGS "-mtune=generic -O3 -Wall -c")

set(CMAKE_CXX_FLAGS_DEBUG "${CCOMMON_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g3 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CCOMMON_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")

# Open SSL
find_package(OpenSSL REQUIRED)

# Not reccomended but I do it anyway
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sleepy_discord/*.c*")
# why static >_>
add_library(sleepy-discord STATIC "${SOURCE_FILES}")

target_include_directories(sleepy-discord PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/sleepy_discord/" "${CMAKE_CURRENT_SOURCE_DIR}/include/sleepy_discord/IncludeNonexistent/")
target_link_libraries(sleepy-discord "${OPENSSL_SSL_LIBRARY}")
