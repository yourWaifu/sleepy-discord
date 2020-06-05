#pragma once
#include <uWS/uWS.h>

#ifndef NONEXISTENT_UWEBSOCKETS
namespace SleepyDiscord {
	typedef uWS::WebSocket<uWS::CLIENT>* WebsocketConnection;
}
#endif