#include "Vec2.h"


std::string stringify (const Vec2& vec2) {
    std::string str;

    str += '<';
    str += std::to_string(vec2.x);
    str += ", ";
    str += std::to_string(vec2.y);
    str += '>';

    return str;
}