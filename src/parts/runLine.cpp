#include <fstream>

#include "../core/Binary.h"
#include "../core/generate.h"
#include "../core/push.h"

#include "runLine.h"


int64_t bind::runLine::get (int64_t x, int64_t y) {
    printf("get: %ld %ld\n", x, y);
    return 0;
}

int64_t bind::runLine::put (int64_t x, int64_t y, int64_t value, uint64_t cursorPacked) {
    printf("put: %ld %ld %ld %s\n", x, y, value, stringify(unpack(cursorPacked)).c_str());
    return 0;
}

int64_t bind::runLine::readInt64 () {
    int64_t value;
    auto count = scanf("%ld", &value);

    if (count <= 0) {
        return -1;

        // fprintf(stderr, "could not read int64 from stdin\n");
        // exit(1);
    }

    return value;
}

int64_t bind::runLine::readChar () {
    char value;
    auto count = scanf("%c", &value);

    if (count <= 0) {
        return -1;

        // fprintf(stderr, "could not read char from stdin\n");
        // exit(1);
    }

    return value;
}

int64_t bind::runLine::writeChar (int64_t value) {
    printf("%c", static_cast<char>(value & 0xff));
    return 0; // to force a call, otherwise it's a jump // needed? doesn't printf do the ret itself?
}

int64_t bind::runLine::writeInt64 (int64_t value) {
    printf("%ld ", value);
    return 0;
}

int64_t bind::runLine::rand4 () {
    return std::rand() & 0x3;
}

void part::runLine (const std::string& fileName) {
    std::ifstream file { fileName };
    std::string source { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

    std::vector<PathletEntry> pathletEntries;

    bool stringMode = false;
    for (size_t index = 0; index < 80 && index < source.length() && source[index] != '\n'; index++) {
        if (stringMode) {
            if (source[index] == '"') {
                stringMode = false;
            }
        } else {
            if (source[index] == '"') {
                stringMode = true;
            }
        }

        pathletEntries.push_back({
            { { static_cast<int8_t>(index), 0 }, { 1, 0 } },
            source[index],
            stringMode
        });
    }

    Path path { std::move(pathletEntries) };

    int64_t stash[16];

    StaticBindings staticBindings {
        stash,
        nullptr,
        bind::runLine::get,
        bind::runLine::put,
        bind::runLine::readInt64,
        bind::runLine::readChar,
        bind::runLine::writeInt64,
        bind::runLine::writeChar,
        bind::runLine::rand4
    };

    std::vector<uint8_t> bytes;
    std::vector<PathLink> pathLinks;

    push::init(bytes);

    generate(path, staticBindings, bytes, pathLinks);

    int64_t stack[256];
    int64_t offset { 0 };

    Binary binary;
    binary.write(bytes);

    // {
    //     FILE* fd = fopen("binary", "wb");
    //     fwrite(code, 1, bytes.size(), fd);
    //     fclose(fd);
    // }

    // objdump -D -Mintel,x86-64 -b binary -m i386 binary

    binary.call(stack, offset);

    printf("\n");
}
