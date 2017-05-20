#pragma once
#include "http.h"

#ifdef SLEEPY_CUSTOM_SESSION
#include "custom_session.h"
#else
#include "cpr_session.h"
#ifdef NONEXISTENT_CPR
//I don't know
#endif
#endif