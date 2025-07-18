#include <cstring>

#include "parts/readPlayfield.h"
#include "parts/findPathlet.h"
#include "parts/findGraph.h"
#include "parts/findGraphOptimize.h"
#include "parts/runLine.h"
#include "parts/run.h"


void printUsage (char* argv[]) {
    fprintf(
        stderr,
        "Usage:\n"
        " %s [--stack-size 4096] <bf-file>\n"
        "\n"
        "Test helpers:\n"
        " %s [--never-opt] [--always-opt] [--stack-size 4096] <bf-file>\n"
        "\n"
        " %s read-playfield <bf-file>\n"
        " %s find-pathlet <bf-file>\n"
        " %s find-graph <bf-file>\n"
        " %s find-graph-optimize <bf-file>\n"
        " %s run-line <bf-file>\n",
        argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]
    );
}

int main (int argc, char* argv[]) {
    if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            printUsage(argv);
        } else {
            part::run(argv[1]);
        }

        return 0;
    }

    if (argc == 3) {
        if (strcmp(argv[1], "read-playfield") == 0) {
            part::readPlayfield(argv[2]);
            return 0;
        }

        if (strcmp(argv[1], "find-pathlet") == 0) {
            part::findPathlet(argv[2]);
            return 0;
        }

        if (strcmp(argv[1], "find-graph") == 0) {
            part::findGraph(argv[2]);
            return 0;
        }

        if (strcmp(argv[1], "find-graph-optimize") == 0) {
            part::findGraphOptimize(argv[2]);
            return 0;
        }

        if (strcmp(argv[1], "run-line") == 0) {
            part::runLine(argv[2]);
            return 0;
        }
    }

    part::RunOptions runOptions;

    auto index = 0;
    for (index = 1; index < argc - 1; index++) {
        if (strcmp(argv[index], "--never-opt") == 0) {
            runOptions.optimizationStrat = part::OptimizationStrat::Never;
            continue;
        }

        if (strcmp(argv[index], "--always-opt") == 0) {
            runOptions.optimizationStrat = part::OptimizationStrat::Always;
            continue;
        }

        if (strcmp(argv[index], "--stack-size") == 0) {
            index++;

            if (index >= argc) {
                fprintf(stderr, "Expected a value following --stack-size\n");
                return 1;
            }

            char* end;
            int64_t stackSize = strtol(argv[index], &end, 10);
            if (*end || stackSize < 1 || stackSize > (1 << 24)) {
                fprintf(stderr, "Expected stack-size to be a number between 1 and 16777216; got '%s' instead\n", argv[2]);
                return 1;
            }

            runOptions.stackSize = stackSize;
            continue;
        }

        fprintf(stderr, "Unexpected argument '%s'\n", argv[index]);
        printUsage(argv);
        return 1;
    }

    if (index >= argc) {
        fprintf(stderr, "Expected a source file\n");
        printUsage(argv);
        return 1;
    }

    part::run(argv[index], runOptions);
}