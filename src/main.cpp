#include <cstring>

#include "parts/readPlayfield.h"
#include "parts/findPathlet.h"
#include "parts/findGraph.h"
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

        if (strcmp(argv[1], "run-line") == 0) {
            part::runLine(argv[2]);
            return 0;
        }

        fprintf(stderr, "Usage: %s read-playfield/find-pathlet/find-graph/run-line <bf-file>\n", argv[0]);
        return 1;
    }

    if (argc == 4) {
        if (strcmp(argv[1], "--stack-size") == 0) {
            char* end;
            int64_t size = strtol(argv[2], &end, 10);
            if (*end || size < 1 || size > (1 << 24)) {
                fprintf(stderr, "Expect stack-size to be a number between 1 and 16777216; got '%s' instead\n", argv[2]);
                return 1;
            }

            part::run(argv[3], static_cast<int32_t>(size));
            return 0;
        }

        fprintf(stderr, "Usage: %s [read-playfield/find-pathlet/find-graph/run-line] --stack-size <stack-size> <bf-file>\n", argv[0]);
        return 1;
    }

    fprintf(stderr, "Usage: %s [read-playfield/find-pathlet/find-graph/run-line] <bf-file>\n", argv[0]);
    return 1;
}