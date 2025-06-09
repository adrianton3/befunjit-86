#include "../core/alloc.h"
#include "../core/generate.h"
#include "../core/generateOpt.h"
#include "../core/readPlayfield.h"

#include "run.h"


Playfield playfield;
Boolfield boolfield;

uint64_t putCursorPacked;
bool compileRequest;

int64_t bind::run::get (int64_t x, int64_t y) {
    return Playfield::isWithinBounds(x, y) ? playfield.getAtUnsafe(x, y) : 0;
}

int64_t bind::run::put (int64_t x, int64_t y, int64_t value, uint64_t cursorPacked) {
    putCursorPacked = cursorPacked;
    if (Playfield::isWithinBounds(x, y)) {
        playfield.setAtUnsafe(x, y, value);
        if (boolfield.getAtUnsafe(x, y)) {
            compileRequest = true;
            return 1;
        }
    }

    return 0;
}

int64_t bind::run::readInt64 () {
    int64_t value;
    auto count = scanf("%ld", &value);

    if (count <= 0) {
        fprintf(stderr, "could not read int64 from stdin\n");
        exit(1);
    }

    return value;
}

int64_t bind::run::readChar () {
    char value;
    auto count = scanf("%c", &value);

    if (count <= 0) {
        fprintf(stderr, "could not read char from stdin\n");
        exit(1);
    }

    return value;
}

int64_t bind::run::writeChar (int64_t value) {
    printf("%c", static_cast<char>(value & 0xff));
    return 0; // to force a call, otherwise it's a jump // needed? doesn't printf do the ret itself?
}

int64_t bind::run::writeInt64 (int64_t value) {
    printf("%ld ", value);
    return 0;
}

int64_t bind::run::rand4 () {
    return std::rand() & 0x3;
}

void part::run (const std::string& file, RunOptions runOptions) {
    readPlayfield(file, playfield);

    int64_t stash[16];

    StaticBindings staticBindings {
        stash,
        playfield.getData(),
        bind::run::get,
        bind::run::put,
        bind::run::readInt64,
        bind::run::readChar,
        bind::run::writeInt64,
        bind::run::writeChar,
        bind::run::rand4
    };

    std::vector<int64_t> stack;
    stack.resize(runOptions.stackSize * 2, 0);
    int64_t offset { 0 };
    stash[1] = offset;

    compileRequest = true;
    putCursorPacked = pack({ { 79, 0 }, { 1, 0 }});

    while (compileRequest) {
        compileRequest = false;

        offset = stash[1];

        auto graph = findGraph(playfield, unpack(putCursorPacked));

        reset(graph, boolfield);

        std::vector<uint8_t> bytes;

        if (runOptions.optimize) {
            generateOpt(graph, staticBindings, boolfield, bytes);
        } else {
            generate(graph, staticBindings, bytes);
        }

        uint8_t* code = alloc(bytes);

        // {
        //     FILE* fd = fopen("binary", "wb");
        //     fwrite(code, 1, bytes.size(), fd);
        //     fclose(fd);
        // }

        call(code, &stack[runOptions.stackSize], offset);

        if (compileRequest) {
            dealloc(code, bytes.size());
        }
    }

    printf("\n");
}