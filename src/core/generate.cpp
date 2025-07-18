#include "push.h"

#include "generate.h"


void generate (
    const Path& path,
    const StaticBindings& staticBindings,
    std::vector<uint8_t>& bytes,
    std::vector<PathLink>& pathLinks
) {
    int64_t loopbackIndex = -1;

    for (size_t index = 0; index < path.entries.size(); index++) {
        if (static_cast<int16_t>(index) == path.loopbackIndex) [[unlikely]] {
            loopbackIndex = bytes.size();
        }

        const auto& entry = path.entries[index];

        if (entry.stringMode) [[unlikely]] {
            push::value(bytes, entry.value);
            continue;
        }

        if (entry.value >= '0' && entry.value <= '9') {
            push::value(bytes, entry.value - '0');
            continue;
        }

        switch (entry.value) {
            case '+': push::add(bytes); break;
            case '-': push::sub(bytes); break;
            case '*': push::mul(bytes); break;
            case '/': push::div(bytes); break;
            case '%': push::mod(bytes); break;

            case '!': push::not_(bytes); break;
            case '`': push::gt(bytes); break;

            case ':': push::dup(bytes); break;
            case '\\': push::swap(bytes); break;
            case '$': push::drop(bytes); break;

            case 'g': push::get(bytes, staticBindings.stash, staticBindings.get); break;
            case 'p': push::put(bytes, staticBindings.stash, staticBindings.put, entry.cursor); break;

            case '.': push::write(bytes, staticBindings.stash, staticBindings.writeInt64); break;
            case ',': push::write(bytes, staticBindings.stash, staticBindings.writeChar); break;

            case '&': push::read(bytes, staticBindings.stash, staticBindings.readInt64); break;
            case '~': push::read(bytes, staticBindings.stash, staticBindings.readChar); break;

            case '_': [[fallthrough]];
            case '|': push::if_(bytes, path, pathLinks); break;

            case '?': push::rand(bytes, staticBindings.stash, staticBindings.rand4, path, pathLinks); break;

            case '@': push::end(bytes, staticBindings.stash); return;

            default: break;
        }
    }

    if (loopbackIndex >= 0) [[unlikely]] {
        push::loopback(bytes, loopbackIndex);
    }
}

void generate (const Graph& graph, const StaticBindings& staticBindings, std::vector<uint8_t>& bytes) {
    std::vector<PathLink> pathLinks;

    push::init(bytes);

    // jump to the main path, wherever that may end up in memory
    push::jump(bytes, graph.start, pathLinks);

    for (auto& entry : graph.map) {
        auto& path = entry.second;
        path->startIndex = bytes.size();
        generate(*path, staticBindings, bytes, pathLinks);
    }

    for (const auto& pathLink : pathLinks) {
        const auto offset = pathLink.next->startIndex - pathLink.patchIndex - 4;
        writeOffset32(bytes, pathLink.patchIndex, offset);
    }
}