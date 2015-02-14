#include "util/Timer.h"
#include "Rally.h"
#include <stdexcept>

namespace Rally { namespace Util {

    Timer::Timer() {
        reset();
    }

    Timer::~Timer() {
    }

    void Timer::reset() {
#ifdef PLATFORM_WINDOWS
        lastTime = timeGetTime();
#else
        if(gettimeofday(&lastTime, NULL) < 0) {
            throw std::runtime_error("gettieofday() failed in Timer.");
        }
#endif
    }

    float Timer::getElapsedSeconds() const {
        // General note: Time may run "backwards" for a couple of reasons.
        // (I think) the functions below are suceptible to that.
        // The reasons include synchronization issues on multi-core processors
        // and the user changing time - or even the OS setting network time.
#ifdef PLATFORM_WINDOWS
        DOWRD time = timeGetTime();

        DWORD difference;
        // Account for wrap-around
        if(lastTime >  0x7FFFFFFF && time < 0x7FFFFFFF) {
            difference = (0xFFFFFFFF - lastTime) + time;
        } else {
            if(time < lastTime) {
                return 0;
            }

            difference = time - lastTime;
        }

        // Return ms to sec
        return ((float) difference) / 1000.0;
#else
        // We don't worry about wrap-around here...
        timeval time;
        if(gettimeofday(&time, NULL) < 0) {
            throw std::runtime_error("gettieofday() failed in Timer.");
        }

        int signedUsecDifference = time.tv_usec - lastTime.tv_usec; // Might be negative!
        float result = ((float) (time.tv_sec - lastTime.tv_sec)) + 0.000001f * signedUsecDifference;

        // Clamp result to [0, inf)
        return (result < 0) ? 0 : result;
#endif
    }

} }
