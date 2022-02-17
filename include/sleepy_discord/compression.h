#pragma once

#if defined(EXISTENT_ZLIB_NG) || defined(EXISTENT_ZLIB)
#include "zlib_compression.h"
#else
#include "generic_compression.h"
#endif // EXISTENT_ZLIB_NG