#include "util/Profiler.h"

#include "GL/gl.h"

#include <stdexcept>
#include <streambuf>

namespace Rally { namespace Util {

    Profiler::Profiler() :
            cycleModulus(1),
            cycleModulusCounter(1) {
        reset();
    }

    Profiler::~Profiler() {
    }

    void Profiler::reset() {
        glFinish();
        timer.reset();
    }

    void Profiler::checkpoint(const std::string& name) {
        glFinish();
        times[name] = timer.getElapsedSeconds();
    }

    void Profiler::printTimes() {
        // Count 1, 2, 3, 1, 2, 3 or similar...
        if((cycleModulusCounter % cycleModulus) == 0) {
            cycleModulusCounter = 1;
            // Fall throught...
        } else {
            cycleModulusCounter++;
            return;
        }

        std::ostringstream out;
        out.precision(4);

        glFinish();
        float totalTime = timer.getElapsedSeconds();

        out << "Profiler report, with a total cycle time of " << totalTime << ", yielding " << (1.0f / totalTime) << " Hz." << std::endl;

        // Print out all middle steps
        float lastTime = 0.0f;
        for(std::map<const std::string, float>::const_iterator timeIterator = times.begin(); timeIterator != times.end(); ++timeIterator) {
            float timeDiff = timeIterator->second - lastTime;
            if(timeDiff < 0.0f) {
                timeDiff = 0.0f;
            }

            out << " - " << timeIterator->first;
            out << ": " << (100.0f * timeDiff / totalTime);
            out << " % (" << timeDiff << " seconds)" << std::endl;

            lastTime = timeIterator->second;
        }

        std::cout << out.str();
        std::cout.flush();
    }

    void Profiler::setCycleModulus(int cycleModulus) {
        this->cycleModulus = cycleModulus;
    }

    Profiler Profiler::instance;

} }
