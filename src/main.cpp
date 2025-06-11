#include <cstring>

#include "parts/readPlayfield.h"
#include "parts/findPathlet.h"
#include "parts/findGraph.h"
#include "parts/findGraphOptimize.h"
#include "parts/runLine.h"
#include "parts/run.h"


int main (int argc, char** argv) {
    if (argc == 2) {
        part::run(argv[1]);
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

        if (strcmp(argv[1], "--no-opt") == 0) {
            part::run(argv[2],{ 4096, false });
            return 0;
        }
    }

    if (argc == 4) {
        if (strcmp(argv[1], "--stack-size") == 0) {
            char* end;
            int64_t size = strtol(argv[2], &end, 10);
            if (*end || size < 1 || size > (1 << 24)) {
                fprintf(stderr, "Expect stack-size to be a number between 1 and 16777216; got '%s' instead\n", argv[2]);
                return 1;
            }

            part::run(argv[3], { static_cast<int32_t>(size), true });
            return 0;
        }
    }

    if (argc == 5) {
        if (strcmp(argv[1], "--stack-size") == 0) {
            char* end;
            int64_t size = strtol(argv[2], &end, 10);
            if (*end || size < 1 || size > (1 << 24)) {
                fprintf(stderr, "Expect stack-size to be a number between 1 and 16777216; got '%s' instead\n", argv[2]);
                return 1;
            }

            if (strcmp(argv[3], "--no-opt") == 0) {
                fprintf(stderr, "Usage: %s [read-playfield/find-pathlet/find-graph/run-line] --stack-size <stack-size> --no-opt <bf-file>\n", argv[0]);
                return 1;
            }

            part::run(argv[4], { static_cast<int32_t>(size), false });
            return 0;
        }

        if (strcmp(argv[1], "--no-opt") == 0) {
            if (strcmp(argv[2], "--stack-size") == 0) {
                char* end;
                int64_t size = strtol(argv[3], &end, 10);
                if (*end || size < 1 || size > (1 << 24)) {
                    fprintf(stderr, "Expect stack-size to be a number between 1 and 16777216; got '%s' instead\n", argv[3]);
                    return 1;
                }

                part::run(argv[4], { static_cast<int32_t>(size), false });
                return 0;
            }
        }
    }

    fprintf(
        stderr,
        "Usage:\n"
        " %s <bf-file>\n"
        " %s --stack-size 4096 <bf-file>\n"
        "\n"
        "Test helpers:\n"
        " %s --no-opt <bf-file>\n"
        " %s --no-opt --stack-size 4096 <bf-file>\n"
        "\n"
        " %s read-playfield <bf-file>\n"
        " %s find-pathlet <bf-file>\n"
        " %s find-graph <bf-file>\n"
        " %s find-graph-optimize <bf-file>\n"
        " %s run-line <bf-file>\n",
        argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]
    );
    return 1;
}