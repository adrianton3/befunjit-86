#ifndef CURSOR_H
#define CURSOR_H

#include <string>

#include "Field.h"
#include "Vec2.h"


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

    bool operator== (const Cursor& rhs) const {
        return location == rhs.location && increment == rhs.increment;
    }
};

uint64_t pack (Cursor);

Cursor unpack (uint64_t);

template<>
struct std::hash<Cursor> {
    std::size_t operator() (const Cursor& cursor) const noexcept {
        return pack(cursor);
    }
};

std::string stringify (const Cursor& cursor);


#endif //CURSOR_H
