add_library(${PROJECT_NAME}

attachment.cpp
channel.cpp
client.cpp
cpr_session.cpp
default_functions.cpp
discord_object_interface.cpp
embed.cpp
endpoints.cpp
experimental.cpp
invite.cpp
json_wrapper.cpp
json.c
message.cpp
permissions.cpp
sd_error.cpp
server.cpp
user.cpp
voice.cpp
webhook.cpp
${WEBSOCKET_IMPL}_websocket.cpp

attachment.h
channel.h
client.h
cpr_session.h
custom_session.h
discord_object_interface.h
embed.h
error.h
experimental.h
gateway.h
http.h
invite.h
json.h
message.h
permissions.h
server.h
session.h
sleepy_discord.h
standard_config.h
standard_config_header.h
user.h
voice.h
webhook.h
${WEBSOCKET_IMPL}_websocket.h
)

if(BOOST_NOT_FOUND)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASIO_STANDALONE)
endif()

# add includes of the dependencies here

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
