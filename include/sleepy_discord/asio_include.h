#pragma once

#ifdef _WIN32
#include <winsock2.h>
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include <netinet/in.h>
#endif

#if !defined(SLEEPY_USE_BOOST) && !defined(EXISTENT_BOOST_ASIO)
	#define ASIO_STANDALONE
	#include "websocketpp_common.h"
	#ifdef NONEXISTENT_WEBSOCKETPP
		#include <asio.hpp>
		#ifdef NONEXISTENT_ASIO
			#undef ASIO_STANDALONE
			#define SLEEPY_USE_BOOST
		#endif
	#else
		namespace asio {
			using namespace websocketpp::lib::asio;
		}
	#endif
#endif

#if defined(SLEEPY_USE_BOOST) || defined(EXISTENT_BOOST_ASIO)
	#include <boost/asio.hpp>
	#ifndef NONEXISTENT_BOOST_ASIO
		#undef NONEXISTENT_ASIO
		namespace asio {
			using namespace boost::asio;
			using boost::system::error_code;
		}
		#ifdef ASIO_STANDALONE
			#undef ASIO_STANDALONE
		#endif
	#endif
#endif