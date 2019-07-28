#pragma once
#include "udp.h"

#ifdef SLEEPY_CUSTOM_UDP_CLIENT
#include "custom_udp_client.h"
#elif defined(SLEEPY_UDP_CLIENT)
typedef SLEEPY_UDP_CLIENT UDPClient
#elif defined(SLEEPY_DISCORD_CMAKE)
	#if defined(EXISTENT_ASIO)
		#include "asio_udp.h"
	#else
		#include "custom_udp_client.h"
	#endif
#else
	#include "asio_udp.h"
	#ifdef NONEXISTENT_ASIO
		#include "custom_udp_client.h"
	#endif
#endif
