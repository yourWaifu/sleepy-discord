#pragma once
#define ASIO_STANDALONE

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#include <asio.hpp>
#ifdef NONEXISTENT_ASIO
#undef NONEXISTENT_ASIO
#undef ASIO_STANDALONE
#include <boost/asio.hpp>
#endif