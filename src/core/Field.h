#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <cstdint>
#include <string>
#include <vector>


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

typedef Field<uint8_t> Boolfield;
typedef Field<int64_t> Playfield;

std::string stringify (const Playfield& playfield);


#endif //PLAYFIELD_H
