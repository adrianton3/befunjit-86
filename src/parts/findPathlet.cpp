#include <iostream>

#include "../core/Field.h"
#include "../core/Pathlet.h"
#include "../core/readPlayfield.h"

#include "findPathlet.h"


void part::findPathlet (const std::string& file) {
    Playfield playfield;
    readPlayfield(file, playfield);

    const auto pathlet = findPathlet(playfield, { 0, 0, 1, 0 });

    std::cout << stringify(pathlet);
}
