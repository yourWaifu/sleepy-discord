# Distributed under the OSI-approved MIT License.  See accompanying
# file LICENSE or https://github.com/Crascit/DownloadProject for details.

cmake_minimum_required(VERSION 2.8.2)

project(websocketpp-download NONE)

include(ExternalProject)
ExternalProject_Add(websocketpp-download
                    GIT_REPOSITORY;https://github.com/zaphoyd/websocketpp.git;GIT_TAG;master;UPDATE_DISCONNECTED;1
                    SOURCE_DIR          "/home/hao-qi/Documents/repositories/sleepy-discord/deps/websocketpp"
                    BINARY_DIR          "/home/hao-qi/Documents/repositories/sleepy-discord/buildtools/websocketpp-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
)
