#include <cstdio>
#include <memory>
#include <sys/mman.h>

#include "Field.h"
#include "Vec2.h"

#include "readPlayfield.h"


std::unique_ptr<char[]> readFile (const std::string& fileName, size_t maxSize) {
    FILE* file = fopen(fileName.c_str(), "r");

    if (!file) {
        printf("could not open file %s\n", fileName.c_str());
        exit(1);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);

    const auto fileSizeRaw = ftell(file);

    if (fileSizeRaw == -1) {
        printf("could not get file size of %s\n", fileName.c_str());
    }

    const auto fileSize = static_cast<size_t>(fileSizeRaw);
    const auto bufferSize = fileSize < maxSize ? fileSize : maxSize;

    rewind(file);

    auto source = std::make_unique<char[]>(bufferSize + 1);

    auto readCount = fread(source.get(), 1, bufferSize, file);

    if (readCount != bufferSize) {
        printf("could not entirely read %s\n", fileName.c_str());
        exit(1);
        return nullptr;
    }

    fclose(file);

    source.get()[bufferSize] = '\0'; // close the lid

    return source;
}

void parse (const char source[], Playfield& playfield) {
    playfield.reset(' ');

    Vec2 location { 0, 0 };
    auto p = 0;

    while (true) {
        const auto ch = source[p];

        if (ch == '\0') {
            break;
        }

        if (ch == '\n') {
            if (location.y == Playfield::height - 1) {
                break;
            }

            location.x = 0;
            location.y++;
        } else {
            if (location.x < Playfield::width) {
                playfield.setAtUnsafe(location.x, location.y, source[p]);
                location.x++;
            }
        }

        p++;
    }
}

void readPlayfield (const std::string& fileName, Playfield& playfield) {
    const auto source = readFile(fileName, Playfield::width * Playfield::height * 4);
    parse(source.get(), playfield);
}
