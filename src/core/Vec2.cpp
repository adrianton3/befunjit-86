#include <format>

#include "Vec2.h"


std::string stringify (const Vec2& vec2) {
    return std::format("<{}, {}>", vec2.x, vec2.y);
}