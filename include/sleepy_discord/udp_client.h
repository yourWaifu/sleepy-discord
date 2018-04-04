#pragma once
#include "udp.h"

#ifdef SLEEPY_CUSTOM_UDP_CLIENT
#include "custom_udp_client.h"
#elif defined(SLEEPY_UDP_CLIENT)
typedef SLEEPY_UDP_CLIENT UDPClient
#else
#include "asio_udp.h"
#ifdef NONEXISTENT_ASIO
#include "custom_udp_client.h"
#endif
#endif