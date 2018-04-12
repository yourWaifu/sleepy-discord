#pragma once
#define ASIO_STANDALONE

#ifdef _WIN32
#include <winsock2.h>
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include <netinet/in.h>
#endif

#include <asio.hpp>
#ifdef NONEXISTENT_ASIO
#undef ASIO_STANDALONE
#include <boost/asio.hpp>
#ifndef NONEXISTENT_BOOST_ASIO
#undef NONEXISTENT_ASIO
namespace asio = boost::asio;
#endif
#endif