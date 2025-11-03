#include <format>
#include <utility>

#include "Vec2.h"

#include "Field.h"


std::string stringify (const Playfield& playfield) {
    struct Nonprintable {
        Vec2 location;
        int64_t value;
    };

    std::string str;
    std::vector<Nonprintable> nonprintables;

    for (int8_t y = 0; y < Playfield::height; y++) {
        for (int8_t x = 0; x < Playfield::width; x++) {
            const auto value = playfield.getAtUnsafe(x, y);

            if (std::in_range<std::int32_t>(value) && std::isprint(static_cast<int32_t>(value))) {
                str += static_cast<char>(value);
            } else {
                nonprintables.push_back({ { x, y }, value });
                str += 'X';
            }
        }

        str += '\n';
    }

    if (!nonprintables.empty()) {
        str += '\n';

        str += std::format("playfield contains {} value(s) that cannot be printed as chars:\n", nonprintables.size());

        for (const auto& nonprintable : nonprintables) {
            str += std::format("{}: {}\n", stringify(nonprintable.location), nonprintable.value);
        }
    }

    return str;
}
