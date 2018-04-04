#pragma once
#include "websockets.h"

#ifdef SLEEPY_CUSTOM_WEBSOCKET_CLIENT
#include "custom_websockets_client.h"
#else
#include "websocketpp_websocket.h"
#ifdef NONEXISTENT_WEBSOCKETPP
#include "uwebsockets_websocket.h"
#ifdef NONEXISTENT_UWEBSOCKETS
//last resort
#include "custom_session.h"
#endif
#endif
#endif