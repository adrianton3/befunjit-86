#ifndef GENERATEOPT_H
#define GENERATEOPT_H

#include "Graph.h"


void generateOpt (const Path& path, const StaticBindings& staticBindings, std::vector<uint8_t>& bytes, std::vector<PathLink>& pathLinks);

void generateOpt (Graph& graph, const StaticBindings& staticBindings, std::vector<uint8_t>& bytes);


#endif //GENERATEOPT_H