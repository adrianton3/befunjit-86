#ifndef GENERATE_H
#define GENERATE_H

#include <cstdint>
#include <vector>

#include "Graph.h"
#include "Path.h"
#include "StaticBindings.h"


void generate (const Path& path, const StaticBindings& staticBindings, std::vector<uint8_t>& bytes, std::vector<PathLink>& pathLinks);

void generate (Graph& graph, const StaticBindings& staticBindings, std::vector<uint8_t>& bytes);


#endif //GENERATE_H
