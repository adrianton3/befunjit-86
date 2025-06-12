#include <cstdio>
#include <cstdint>
#include <memory>
#include <string>
#include <sys/mman.h>
#include <vector>


struct Vec2 {
    int8_t x;
    int8_t y;
};

// ---

template<typename T>
class Field {
public:
    static constexpr uint8_t width = 80;
    static constexpr uint8_t height = 25;

    Field () {
        flat.resize(width * height);
    }

    T getAtUnsafe (int64_t x, int64_t y) const {
        return flat[y * width + x];
    }

    void setAtUnsafe (int64_t x, int64_t y, T value) {
        flat[y * width + x] = value;
    }

    void reset (T value) {
        std::fill(flat.begin(), flat.end(), value);
    }

    static bool isWithinBounds (int64_t x, int64_t y) {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

private:
    std::vector<T> flat;
};

typedef Field<int64_t> Playfield;


// ---

struct Cursor {
    Vec2 location;
    Vec2 increment;

    void directUp () {
        increment.x = 0;
        increment.y = -1;
    }

    void directLeft () {
        increment.x = -1;
        increment.y = 0;
    }

    void directDown () {
        increment.x = 0;
        increment.y = 1;
    }

    void directRight () {
        increment.x = 1;
        increment.y = 0;
    }

    void advance () {
        if (location.x == 0 && increment.x < 0) [[unlikely]] {
            location.x = Playfield::width - 1;
            return;
        }

        if (location.x == Playfield::width - 1 && increment.x > 0) [[unlikely]] {
            location.x = 0;
            return;
        }

        if (location.y == 0 && increment.y < 0) [[unlikely]] {
            location.y = Playfield::height - 1;
            return;
        }

        if (location.y == Playfield::height - 1 && increment.y > 0) [[unlikely]] {
            location.y = 0;
            return;
        }

        location.x += increment.x;
        location.y += increment.y;
    }
};

// ---

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

// ---

Playfield playfield;

int64_t get (int64_t x, int64_t y) {
    return Playfield::isWithinBounds(x, y) ? playfield.getAtUnsafe(x, y) : 0;
}

void put (int64_t x, int64_t y, int64_t value) {
    if (Playfield::isWithinBounds(x, y)) {
        playfield.setAtUnsafe(x, y, value);
    }
}

int64_t readInt64 () {
    int64_t value;
    auto count = scanf("%ld", &value);

    if (count <= 0) {
        fprintf(stderr, "could not read int64 from stdin\n");
        exit(1);
    }

    return value;
}

int64_t readChar () {
    char value;
    auto count = scanf("%c", &value);

    if (count <= 0) {
        fprintf(stderr, "could not read char from stdin\n");
        exit(1);
    }

    return value;
}

void writeChar (int64_t value) {
    printf("%c", static_cast<char>(value & 0xff));
}

void writeInt64 (int64_t value) {
    printf("%ld ", value);
}

int64_t rand4 () {
    return std::rand() & 0x3;
}

// ---

void run (const std::string& file, int32_t stackSize) {
    readPlayfield(file, playfield);

    std::vector<int64_t> stack;
    stack.resize(stackSize * 2, 0);

    int64_t stackPointer = stackSize;

    Cursor cursor { { 0, 0 }, { 1, 0 } };

    bool stringMode = false;

    while (true) {
        const auto value = playfield.getAtUnsafe(cursor.location.x, cursor.location.y);

        if (stringMode) [[unlikely]] {
            if (value == '"') {
                stringMode = false;
            } else {
                stack[stackPointer] = value;
                stackPointer++;
            }
            cursor.advance();
            continue;
        }

        switch (value) {
            case '0': [[fallthrough]];
            case '1': [[fallthrough]];
            case '2': [[fallthrough]];
            case '3': [[fallthrough]];
            case '4': [[fallthrough]];
            case '5': [[fallthrough]];
            case '6': [[fallthrough]];
            case '7': [[fallthrough]];
            case '8': [[fallthrough]];
            case '9': {
                stack[stackPointer] = value - '0';
                stackPointer++;
                break;
            }

            case '^': cursor.directUp(); break;
            case '<': cursor.directLeft(); break;
            case 'v': cursor.directDown(); break;
            case '>': cursor.directRight(); break;

            case '#': cursor.advance(); break;

            case '+': {
                stack[stackPointer - 2] += stack[stackPointer - 1];
                stackPointer--;
                break;
            }

            case '-': {
                stack[stackPointer - 2] -= stack[stackPointer - 1];
                stackPointer--;
                break;
            }

            case '*': {
                stack[stackPointer - 2] *= stack[stackPointer - 1];
                stackPointer--;
                break;
            }

            case '/': {
                stack[stackPointer - 2] /= stack[stackPointer - 1];
                stackPointer--;
                break;
            }

            case '%': {
                stack[stackPointer - 2] %= stack[stackPointer - 1];
                stackPointer--;
                break;
            }

            case '!': {
                stack[stackPointer - 1] = !stack[stackPointer - 1];
                break;
            }

            case '`': {
                stack[stackPointer - 2] = stack[stackPointer - 2] > stack[stackPointer - 1];
                stackPointer--;
                break;
            }

            case '"': {
                stringMode = true;
                break;
            }

            case ':': {
                stack[stackPointer] = stack[stackPointer - 1];
                stackPointer++;
                break;
            }

            case '\\': {
                const auto tmp = stack[stackPointer - 2];
                stack[stackPointer - 2] = stack[stackPointer - 1];
                stack[stackPointer - 1] = tmp;
                break;
            }

            case '$': {
                stackPointer--;
                break;
            }

            case '_': {
                if (stack[stackPointer - 1] == 0) {
                    cursor.directRight();
                } else {
                    cursor.directLeft();
                }
                stackPointer--;
                break;
            }

            case '|': {
                if (stack[stackPointer - 1] == 0) {
                    cursor.directDown();
                } else {
                    cursor.directUp();
                }
                stackPointer--;
                break;
            }

            case 'g': {
                stack[stackPointer - 2] = get(stack[stackPointer - 2], stack[stackPointer - 1]);
                stackPointer--;
                break;
            }

            case 'p': {
                put(stack[stackPointer - 2], stack[stackPointer - 1], stack[stackPointer - 3]);
                stackPointer -= 3;
                break;
            }

            case '.': {
                writeInt64(stack[stackPointer - 1]);
                stackPointer--;
                break;
            }

            case ',': {
                writeChar(stack[stackPointer - 1]);
                stackPointer--;
                break;
            }

            case '&': {
                stack[stackPointer] = readInt64();
                stackPointer++;
                break;
            }

            case '~': {
                stack[stackPointer] = readChar();
                stackPointer++;
                break;
            }

            case '@': return;

            default: break;
        }

        cursor.advance();
    }
}

// ---

int main (int argc, char** argv) {
    if (argc == 2) {
        run(argv[1], 4096);
        return 0;
    }

    fprintf(
        stderr,
        "Usage:\n"
        " %s <bf-file>\n",
        argv[0]
    );
    return 1;
}