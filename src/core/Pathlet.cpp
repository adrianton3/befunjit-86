#include <format>
#include <string>
#include <unordered_map>
#include <utility>

#include "Pathlet.h"


Pathlet findPathlet (const Playfield& playfield, Cursor cursor) {
    std::unordered_map<Cursor, int16_t> map;
    std::vector<PathletEntry> entries;

    int16_t index = 0;
    bool stringMode = false;

    while (true) {
        const auto value = playfield.getAtUnsafe(cursor.location.x, cursor.location.y);

        if (stringMode) {
            // while in string mode don't care about loops
            // we will eventually get out of string mode if going forward
            // and can start caring about loops again
            if (value == '"') {
                stringMode = false;
            } else {
                index++;
            }

            entries.emplace_back(cursor, value, stringMode);
            cursor.advance();

            continue;
        }

        if (value == '^') {
            cursor.directUp();
        } else if (value == '<') {
            cursor.directLeft();
        } else if (value == 'v') {
            cursor.directDown();
        } else if (value == '>') {
            cursor.directRight();
        }

        if (const auto indexMaybe = map.find(cursor); indexMaybe != map.end()) {
            return { std::move(entries), indexMaybe->second };
        }

        map[cursor] = index;

        entries.emplace_back(cursor, value, false);

        if (value == '_' || value == '|' || value == '?' || value == '@') {
            return { std::move(entries), -1 };
        }

        if (value == '#') {
            cursor.advance();
        }

        if (value == '"') {
            stringMode = true;
        }

        cursor.advance();
        index++;
    }

    return { std::move(entries), -1 };
}

std::string stringify (const PathletEntry& entry) {
    std::string str;

    str += std::format("<{}", stringify(entry.cursor.location));

    if (std::in_range<std::int32_t>(entry.value) && std::isprint(static_cast<int32_t>(entry.value))) {
        str += std::format(", '{}'", static_cast<char>(entry.value));
    } else {
        str += std::format(", ({})", entry.value);
    }

    if (entry.stringMode) {
        str += ", \"";
    }

    str += ">";

    return str;
}

std::string stringify (const Pathlet& pathlet) {
    std::string str;

    if (pathlet.loopbackIndex >= 0) {
        str += "looping path\n";
        str += std::format("total length: {}\n", pathlet.entries.size());
        str += std::format("pre-loop length: {}\n", pathlet.loopbackIndex);
    } else {
        str += "simple path\n";
        str += std::format("total length: {}\n", pathlet.entries.size());
    }

    for (const auto& entry : pathlet.entries) {
        str += std::format("{}\n", stringify(entry));
    }

    return str;
}