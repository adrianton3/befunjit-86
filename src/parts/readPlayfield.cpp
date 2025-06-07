#include <iostream>

#include "../core/Field.h"
#include "../core/readPlayfield.h"

#include "readPlayfield.h"


void part::readPlayfield (const std::string& file) {
    Playfield playfield;
    readPlayfield(file, playfield);

    std::cout << stringify(playfield);
}
