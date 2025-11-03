#include <format>
#include <iostream>
#include <utility>
#include <variant>

#include "../core/generateOpt.h"
#include "../core/Graph.h"
#include "../core/Field.h"
#include "../core/readPlayfield.h"

#include "findGraphOptimize.h"


void part::findGraphOptimize (const std::string& file) {
    Playfield playfield;
    readPlayfield(file, playfield);

    const auto graph = findGraph(playfield, { { 79, 0 },  { 1, 0 }});

    std::string str;

    str += std::format("path count: {}\n", graph.map.size());

    for (const auto& [_, path] : graph.map) {
        str += std::format("{}: ", stringify(path->entries[0].cursor.location));

        for (const auto& entry : path->entries) {
            if (std::in_range<std::int32_t>(entry.value) && std::isprint(static_cast<int32_t>(entry.value))) {
                str += static_cast<char>(entry.value);
            } else {
                str += 'X';
            }
        }

        str += std::format(" {}\n", stringify(path->entries.back().cursor.location));

        std::vector<Instr> instrs;
        optimize(path->entries, instrs);

        for (const auto& instr : instrs) {
            str += std::format("{} ", stringify(instr));
        }

        str += '\n';
        str += '\n';
    }

    std::cout << str;
}