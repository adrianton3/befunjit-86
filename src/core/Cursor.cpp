#include "Cursor.h"


uint64_t pack (Cursor cursor) {
    return
       ((cursor.increment.y + 1) << 18) |
       ((cursor.increment.x + 1) << 16) |
       (cursor.location.y << 8) |
       cursor.location.x;
}

Cursor unpack (uint64_t packed) {
    return {
        { static_cast<int8_t>(packed & 0xff), static_cast<int8_t>((packed >> 8) & 0xff) },
        { static_cast<int8_t>(((packed >> 16) & 0x03) - 1), static_cast<int8_t>(((packed >> 18) & 0x03) - 1) }
    };
}

std::string stringify (const Cursor& cursor) {
    std::string str;

    str += '<';
    str += stringify(cursor.location);
    str += ", ";
    str += stringify(cursor.increment);
    str += '>';

    return str;
}