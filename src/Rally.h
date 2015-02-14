#ifndef RALLY_H_
#define RALLY_H

#if _WIN32
#define PLATFORM_WINDOWS
#endif

#if PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#endif // RALLY_H_
