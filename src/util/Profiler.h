#ifndef RALLY_UTIL_PROFILER_H_
#define RALLY_UTIL_PROFILER_H_

#include "util/Timer.h"

#include <string>
#include <map>

namespace Rally { namespace Util {

    class Profiler {
        public:
            Profiler();
            virtual ~Profiler();

            void reset();
            void checkpoint(const std::string& name);
            void printTimes();
            void setCycleModulus(int cycleModulus);

            static Profiler instance;

        private:
            Rally::Util::Timer timer;
            std::map<const std::string, float> times;

            int cycleModulus;
            int cycleModulusCounter;
    };

} }

#endif // RALLY_UTIL_PROFILER_H_
