#ifndef PATHLET_H
#define PATHLET_H

#include <vector>

#include "Cursor.h"
#include "Field.h"


struct PathletEntry {
    Cursor cursor;
    int64_t value;
    bool stringMode;
};

std::string stringify (const PathletEntry& pathletEntry);

struct Pathlet {
    std::vector<PathletEntry> entries;
    int16_t loopbackIndex;
};

Pathlet findPathlet (const Playfield& playfield, Cursor cursor);

std::string stringify (const Pathlet& pathlet);


#endif //PATHLET_H
