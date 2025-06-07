#ifndef VEC2_H
#define VEC2_H

#include <string>


struct Vec2 {
    int8_t x;
    int8_t y;

    bool operator== (const Vec2& rhs) const {
        return x == rhs.x && y == rhs.y;
    };
};

std::string stringify (const Vec2& vec2);


#endif //VEC2_H
