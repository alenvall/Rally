#ifndef RALLY_UTIL_TIMER_H_
#define RALLY_UTIL_TIMER_H_

#include "Rally.h"
#ifndef PLATFORM_WINDOWS
#include <sys/time.h>
#endif

namespace Rally { namespace Util {

    class Timer {
        public:
            Timer();
            virtual ~Timer();

            void reset();
            float getElapsedSeconds() const;

        private:
#ifdef PLATFORM_WINDOWS
            DWORD lastTime;
#else
            timeval lastTime;
#endif
    };

} }

#endif // RALLY_UTIL_TIMER_H_
