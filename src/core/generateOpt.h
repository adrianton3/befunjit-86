#ifndef GENERATEOPT_H
#define GENERATEOPT_H

#include "Graph.h"


void generateOpt (const Path&, const StaticBindings&, const Boolfield&, std::vector<uint8_t>&, std::vector<PathLink>&);

void generateOpt (Graph&, const StaticBindings&, const Boolfield&, std::vector<uint8_t>&);


#endif //GENERATEOPT_H