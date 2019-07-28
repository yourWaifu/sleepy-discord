#pragma once
#define ASIO_STANDALONE

#ifdef _WIN32
#include <winsock2.h>
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include <netinet/in.h>
#endif

#if defined(SLEEPY_DISCORD_CMAKE) && defined(EXISTENT_ASIO)
	#include <asio.hpp>
#else
	#include <asio.hpp>
	#ifdef NONEXISTENT_ASIO
		#undef ASIO_STANDALONE
		#define SLEEPY_USE_BOOST
	#endif
#endif

#if defined(SLEEPY_USE_BOOST) || defined(EXISTENT_BOOST_ASIO)
	#include <boost/asio.hpp>
	#ifndef NONEXISTENT_BOOST_ASIO
		#undef NONEXISTENT_ASIO
		namespace asio = boost::asio;
	#endif
#endif
