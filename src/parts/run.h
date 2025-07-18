#ifndef RUN_H
#define RUN_H

#include <string>


namespace bind::run {

    int64_t get (int64_t x, int64_t y);

    int64_t put (int64_t x, int64_t y, int64_t value, uint64_t cursorPacked);

    int64_t readInt64 ();

    int64_t readChar ();

    int64_t writeChar (int64_t value);

    int64_t writeInt64 (int64_t value);

    int64_t rand4 ();

}

namespace part {

    enum class OptimizationStrat {
        Never,
        Bail,
        Always,
    };

    struct RunOptions {
        RunOptions() : stackSize { 4096 }, optimizationStrat { OptimizationStrat::Bail } {}
        int32_t stackSize;
        OptimizationStrat optimizationStrat;
    };

    void run (const std::string& file, RunOptions runOptions = {});

}


#endif //RUN_H
