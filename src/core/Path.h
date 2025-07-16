#ifndef PATH_H
#define PATH_H

#include <vector>

#include "Pathlet.h"


struct Path;

struct Path {
    Path (const std::vector<PathletEntry>&& _entries, int16_t _loopbackIndex = 0)
    : entries { std::move(_entries) }
    , next0 { nullptr }
    , next1 { nullptr }
    , next2 { nullptr }
    , next3 { nullptr }
    , loopbackIndex { _loopbackIndex }
    , startIndex { 0 }
    {}

    std::vector<PathletEntry> entries;
    Path* next0;
    Path* next1;
    Path* next2;
    Path* next3;
    int16_t loopbackIndex;
    int64_t startIndex;
};

// stringify

struct PathLink {
    int64_t patchIndex;
    Path* next;
};

// stringify


#endif //PATH_H
