#ifndef INTERPRET_H
#define INTERPRET_H

#include <vector>

#include "Cursor.h"
#include "Field.h"
#include "StaticBindings.h"


struct InterpretResult {
    Cursor cursor;
    int64_t offset;
    bool done;
};

InterpretResult interpret (Playfield&, Cursor, int64_t*, int64_t, const StaticBindings&, uint64_t);


#endif //INTERPRET_H
