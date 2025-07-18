#include <functional>

#include "Path.h"

#include "Graph.h"


Graph findGraph (const Playfield& playfield, const Cursor& cursor) {
    std::unordered_map<Cursor, Path*> map;

    const std::function<Path*(const Cursor&)> df = [&](const Cursor& preStart) -> Path* {
        if (const auto& pathMaybe = map.find(preStart); pathMaybe != map.end()) {
            return pathMaybe->second;
        }

        Cursor start = preStart;
        start.advance();
        auto pathlet = findPathlet(playfield, start);

        if (pathlet.loopbackIndex >= 0) {
            auto* path = new Path { std::move(pathlet.entries), pathlet.loopbackIndex };
            map[preStart] = path;
            return path;
        }

        const PathletEntry& pathletEntryEnd = pathlet.entries.back();

        if (pathletEntryEnd.value == '@') {
            auto* path = new Path { std::move(pathlet.entries) };
            map[preStart] = path;
            return path;
        }

        if (pathletEntryEnd.value == '_') {
            auto* path = new Path { std::move(pathlet.entries) };
            map[preStart] = path;

            Cursor preStartRight = pathletEntryEnd.cursor;
            preStartRight.directRight();
            path->next0 = df(preStartRight);

            Cursor preStartLeft = pathletEntryEnd.cursor;
            preStartLeft.directLeft();
            path->next1 = df(preStartLeft);

            return path;
        }

        if (pathletEntryEnd.value == '|') {
            auto* path = new Path { std::move(pathlet.entries) };
            map[preStart] = path;

            Cursor preStartDown = pathletEntryEnd.cursor;
            preStartDown.directDown();
            path->next0 = df(preStartDown);

            Cursor preStartUp = pathletEntryEnd.cursor;
            preStartUp.directUp();
            path->next1 = df(preStartUp);

            return path;
        }

        if (pathletEntryEnd.value == '?') {
            auto* path = new Path { std::move(pathlet.entries) };
            map[preStart] = path;

            Cursor preStartUp = pathletEntryEnd.cursor;
            preStartUp.directUp();
            path->next0 = df(preStartUp);

            Cursor preStartLeft = pathletEntryEnd.cursor;
            preStartLeft.directLeft();
            path->next1 = df(preStartLeft);

            Cursor preStartDown = pathletEntryEnd.cursor;
            preStartDown.directDown();
            path->next2 = df(preStartDown);

            Cursor preStartRight = pathletEntryEnd.cursor;
            preStartRight.directRight();
            path->next3 = df(preStartRight);

            return path;
        }

        return nullptr;
    };

    auto* start = df(cursor);

    return Graph { std::move(map), start };
}

std::string stringify (const Graph& graph) {
    std::string str;

    str += "path count: ";
    str += std::to_string(graph.map.size());
    str += '\n';

    for (const auto& [_, path] : graph.map) {
        str += stringify(path->entries[0].cursor.location);
        str += ": ";

        for (const auto& entry : path->entries) {
            if (std::in_range<std::int32_t>(entry.value) && std::isprint(static_cast<int32_t>(entry.value))) {
                str += static_cast<char>(entry.value);
            } else {
                str += 'X';
            }
        }

        str += " ";
        str += stringify(path->entries.back().cursor.location);
        str += '\n';
    }

    return str;
}

void reset (const Graph& graph, Boolfield& boolfield) {
    boolfield.reset(false);

    for (const auto& [_, path] : graph.map) {
        for (const auto& entry : path->entries) {
            boolfield.setAtUnsafe(entry.cursor.location.x, entry.cursor.location.y, true);
        }
    }
}