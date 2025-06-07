#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>

#include "Path.h"


struct Graph {
    std::unordered_map<Cursor, Path*> map;
    Path* start;

    ~Graph () {
        for (const auto& entry : map) {
            delete entry.second;
        }
    }
};

Graph findGraph (const Playfield& playfield, const Cursor& cursor);

std::string stringify (const Graph& graph);

void reset (const Graph& graph, Boolfield& boolfield);


#endif //GRAPH_H
