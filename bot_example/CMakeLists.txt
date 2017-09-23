add_executable(example example0.cpp)

# todo: all library header paths

target_include_directories(example PUBLIC "..")
target_link_libraries(example
    sleepy_discord
    ${CPR_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET example PROPERTY CXX_STANDARD 11)
set_property(TARGET example PROPERTY CXX_STANDARD_REQUIRED ON)
