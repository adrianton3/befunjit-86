#ifndef GENERATEOPT_H
#define GENERATEOPT_H

#include <variant>

#include "Graph.h"
#include "Instr.h"
#include "StaticBindings.h"


void optimize (const std::vector<PathletEntry>&, std::vector<Instr>&);

void generateOpt (const Path&, const StaticBindings&, const Boolfield&, std::vector<uint8_t>&, std::vector<PathLink>&);

void generateOpt (const Graph&, const StaticBindings&, const Boolfield&, std::vector<uint8_t>&);


#endif //GENERATEOPT_H