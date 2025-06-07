#include <fstream>
#include <iostream>

#include "../core/Graph.h"
#include "../core/Field.h"
#include "../core/readPlayfield.h"

#include "findGraph.h"


void part::findGraph (const std::string& file) {
    Playfield playfield;
    readPlayfield(file, playfield);

    const auto graph = findGraph(playfield, { { 79, 0 },  { 1, 0 }});

    std::cout << stringify(graph);
}