#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>

#include "Path.h"


struct Graph {
    Graph (std::unordered_map<Cursor, Path*>&& _map, Path* _start)
    : map { std::move(_map) }
    , start { _start }
    {}

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
