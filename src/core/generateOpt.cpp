#include <array>
#include <variant>

#include "generate.h"
#include "Pathlet.h"
#include "push.h"

#include "generateOpt.h"


enum class InstrType : uint8_t {
    Push,
    Add, Sub, Mul, Mul1, Div, Mod,
    Not, Gt, Gte, Lt, Lte,
    Dup, Swap, Drop,
    Get, Put,
    ReadInt64, ReadChar,
    WriteInt64, WriteChar,
    If, Rand, End
};

struct Push { int64_t value; };
struct Add {};
struct Sub {};
struct Mul {};
struct Mul1 { int64_t value; };
struct Div {};
struct Mod {};

struct Not {};
struct Gt {};
struct Gte {};
struct Lt {};
struct Lte {};

struct Dup {};
struct Swap {};
struct Drop {};

struct Get {};
struct Put { Cursor cursor; };

struct ReadInt64 {};
struct ReadChar {};
struct WriteInt64 {};
struct WriteChar {};

struct If {};
struct Rand {};
struct End {};

typedef std::variant<
    Push,
    Add, Sub, Mul, Mul1, Div, Mod,
    Not, Gt, Gte, Lt, Lte,
    Dup, Swap, Drop,
    Get, Put,
    ReadInt64, ReadChar,
    WriteInt64, WriteChar,
    If, Rand, End
> Instr;

void translatePass (const std::vector<PathletEntry>& prev, std::vector<Instr>& next) {
    for (const auto& entry : prev) {
        if (entry.stringMode) [[unlikely]] {
            next.emplace_back(Push { entry.value });
            continue;
        }

        if (entry.value >= '0' && entry.value <= '9') {
            next.emplace_back(Push { entry.value - '0' });
        }

        switch (entry.value) {
            case '+': next.emplace_back(Add {}); break;
            case '-': next.emplace_back(Sub {}); break;
            case '*': next.emplace_back(Mul {}); break;
            case '/': next.emplace_back(Div {}); break;
            case '%': next.emplace_back(Mod {}); break;
            case '!': next.emplace_back(Not {}); break;
            case '`': next.emplace_back(Gt {}); break;
            case ':': next.emplace_back(Dup {}); break;
            case '\\': next.emplace_back(Swap {}); break;
            case '$': next.emplace_back(Drop {}); break;
            case 'g': next.emplace_back(Get {}); break;
            case 'p': next.emplace_back(Put { entry.cursor }); break;
            case '.': next.emplace_back(WriteInt64 {}); break;
            case ',': next.emplace_back(WriteChar {}); break;
            case '&': next.emplace_back(ReadInt64 {}); break;
            case '~': next.emplace_back(ReadChar {}); break;
            case '_': [[fallthrough]];
            case '|': next.emplace_back(If {}); break;
            case '?': next.emplace_back(Rand {}); break;
            case '@': next.emplace_back(End {}); return;
            default: break;
        }
    }
}

bool matchesUnsafe (const std::vector<Instr>& instructions, size_t index, InstrType i0, InstrType i1) {
    return instructions[index + 0].index() == static_cast<size_t>(i0) &&
        instructions[index + 1].index() == static_cast<size_t>(i1);
}

bool matchesUnsafe (const std::vector<Instr>& instructions, size_t index, InstrType i0, InstrType i1, InstrType i2) {
    return instructions[index + 0].index() == static_cast<size_t>(i0) &&
        instructions[index + 1].index() == static_cast<size_t>(i1) &&
        instructions[index + 2].index() == static_cast<size_t>(i2);
}

bool matchesUnsafe (const std::vector<Instr>& instructions, size_t index, InstrType i0, InstrType i1, InstrType i2, InstrType i3) {
    return instructions[index + 0].index() == static_cast<size_t>(i0) &&
        instructions[index + 1].index() == static_cast<size_t>(i1) &&
        instructions[index + 2].index() == static_cast<size_t>(i2) &&
        instructions[index + 3].index() == static_cast<size_t>(i3);
}

int64_t getPushValue (Instr push) {
    return std::get<Push>(push).value;
}

uint64_t foldPass (const std::vector<Instr>& prev, std::vector<Instr>& next) {
    const auto indexMaxM0 = prev.size();
    const auto indexMaxM2 = indexMaxM0 - 2;
    const auto indexMaxM3 = indexMaxM0 - 3;
    const auto indexMaxM4 = indexMaxM0 - 4;

    uint64_t rewriteCount = 0;

    size_t index = 0;

    while (index < indexMaxM0) {
        if (index < indexMaxM4) {
            // a+b+
            // (... + a) + b
            // ... + (a + b)
            // (a+b)+
            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Add, InstrType::Push, InstrType::Add)) {
                next.emplace_back(Push { getPushValue(prev[index + 0]) + getPushValue(prev[index + 2]) });
                next.emplace_back(Add {});
                rewriteCount++;
                index += 4;
                continue;
            }

            // a+b-
            // (... + a) - b
            // ... + (a - b)
            // (a-b)+
            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Add, InstrType::Push, InstrType::Sub)) {
                next.emplace_back(Push { getPushValue(prev[index + 0]) - getPushValue(prev[index + 2]) });
                next.emplace_back(Add {});
                rewriteCount++;
                index += 4;
                continue;
            }

            // a-b+
            // (... + -a) + b
            // ... + (-a + b)
            // (b-a)+
            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Sub, InstrType::Push, InstrType::Add)) {
                next.emplace_back(Push { getPushValue(prev[index + 2]) - getPushValue(prev[index + 0]) });
                next.emplace_back(Add {});
                rewriteCount++;
                index += 4;
                continue;
            }

            // a-b-
            // (... + -a) + -b
            // ... + (-a + -b)
            // ... - (a + b)
            // (a+b)-
            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Sub, InstrType::Push, InstrType::Sub)) {
                next.emplace_back(Push { getPushValue(prev[index + 0]) + getPushValue(prev[index + 2]) });
                next.emplace_back(Sub {});
                rewriteCount++;
                index += 4;
                continue;
            }

            // a*b*
            // (... * a) * b
            // ... * (a * b)
            // (a*b)*
            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Mul, InstrType::Push, InstrType::Mul)) {
                next.emplace_back(Push { getPushValue(prev[index + 0]) * getPushValue(prev[index + 2]) });
                next.emplace_back(Mul {});
                rewriteCount++;
                index += 4;
                continue;
            }
        }

        if (index < indexMaxM3) {
            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Push, InstrType::Add)) {
                next.emplace_back(Push { getPushValue(prev[index + 0]) + getPushValue(prev[index + 1]) });
                rewriteCount++;
                index += 3;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Push, InstrType::Sub)) {
                next.emplace_back(Push { getPushValue(prev[index + 0]) - getPushValue(prev[index + 1]) });
                rewriteCount++;
                index += 3;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Push, InstrType::Mul)) {
                next.emplace_back(Push { getPushValue(prev[index + 0]) * getPushValue(prev[index + 1]) });
                rewriteCount++;
                index += 3;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Swap, InstrType::Sub) && getPushValue(prev[index + 0]) == 0) {
                next.emplace_back(Push { -1 });
                next.emplace_back(Mul {});
                rewriteCount++;
                index += 3;
                continue;
            }
        }

        if (index < indexMaxM2) {
            if (matchesUnsafe(prev, index, InstrType::Dup, InstrType::Add)) {
                next.emplace_back(Push { 2 });
                next.emplace_back(Mul {});
                rewriteCount++;
                index += 2;
                continue;
            }
        }

        next.push_back(prev[index]);
        index++;
    }

    return rewriteCount;
}

void finalPass (const std::vector<Instr>& prev, std::vector<Instr>& next) {
    const auto indexMaxM0 = prev.size();
    const auto indexMaxM2 = indexMaxM0 - 2;
    const auto indexMaxM3 = indexMaxM0 - 3;

    size_t index = 0;

    while (index < indexMaxM0) {
        if (index < indexMaxM3) {
            // ab\`!
            // !(b>a)
            // b<=a
            // ab(>=)
            if (matchesUnsafe(prev, index, InstrType::Swap, InstrType::Gt, InstrType::Not)) {
                next.emplace_back(Gte {});
                index += 3;
                continue;
            }
        }

        if (index < indexMaxM2) {
            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Mul)) {
                next.emplace_back(Mul1 { getPushValue(prev[index + 0]) });
                index += 2;
                continue;
            }

            // ab\`
            // b>a
            // a<b
            // ab(<)
            if (matchesUnsafe(prev, index, InstrType::Swap, InstrType::Gt)) {
                next.emplace_back(Lt {});
                index += 2;
                continue;
            }

            // ab`!
            // !(a>b)
            // a<=b
            // ab(<=)
            if (matchesUnsafe(prev, index, InstrType::Gt, InstrType::Not)) {
                next.emplace_back(Lte {});
                index += 2;
                continue;
            }
        }

        next.push_back(prev[index]);
        index++;
    }
}

void generateOpt (
    const Path& path,
    const StaticBindings& staticBindings,
    std::vector<uint8_t>& bytes,
    std::vector<PathLink>& pathLinks
) {
    if (path.loopbackIndex >= 0) {
        // loopback index is hard to keep track of during translation
        // exact instruction to loop back to might also be gone
        // use simple generator instead
        // they're not worth optimizing
        generate(path, staticBindings, bytes, pathLinks);
        return;
    }

    std::vector<Instr> firsts;
    translatePass(path.entries, firsts);

    std::vector<Instr> seconds;
    const auto foldCount = foldPass(firsts, seconds);

    std::vector<Instr> finals;

    if (foldCount > 0) {
        std::vector<Instr> thirds;
        foldPass(seconds, thirds);
        // two passes should be enough

        finalPass(thirds, finals);
    } else {
        finalPass(seconds, finals);
    }

    for (const auto& instr : finals) {
        const auto type = static_cast<InstrType>(instr.index());

        switch (type) {
            case InstrType::Push: push::value(bytes, std::get<Push>(instr).value); break;

            case InstrType::Add: push::add(bytes); break;
            case InstrType::Sub: push::sub(bytes); break;
            case InstrType::Mul: push::mul(bytes); break;
            case InstrType::Mul1: push::mul1(bytes, std::get<Mul1>(instr).value); break;
            case InstrType::Div: push::div(bytes); break;
            case InstrType::Mod: push::mod(bytes); break;

            case InstrType::Not: push::not_(bytes); break;
            case InstrType::Gt: push::gt(bytes); break;
            case InstrType::Gte: push::gte(bytes); break;
            case InstrType::Lt: push::lt(bytes); break;
            case InstrType::Lte: push::lte(bytes); break;

            case InstrType::Dup: push::dup(bytes); break;
            case InstrType::Swap: push::swap(bytes); break;
            case InstrType::Drop: push::drop(bytes); break;

            case InstrType::Get: push::get(bytes, staticBindings.stash, staticBindings.get); break;
            case InstrType::Put: push::put(bytes, staticBindings.stash, staticBindings.put, std::get<Put>(instr).cursor); break;

            case InstrType::ReadInt64: push::read(bytes, staticBindings.stash, staticBindings.readInt64); break;
            case InstrType::ReadChar: push::read(bytes, staticBindings.stash, staticBindings.readChar); break;
            case InstrType::WriteInt64: push::write(bytes, staticBindings.stash, staticBindings.writeInt64); break;
            case InstrType::WriteChar: push::write(bytes, staticBindings.stash, staticBindings.writeChar); break;

            case InstrType::If: push::if_(bytes, path, pathLinks); break;
            case InstrType::Rand: push::rand(bytes, staticBindings.stash, staticBindings.rand4, path, pathLinks); break;
            case InstrType::End: push::end(bytes, staticBindings.stash); return;

            default: break;
        }
    }
}

void generateOpt (Graph& graph, const StaticBindings& staticBindings, std::vector<uint8_t>& bytes) {
    std::vector<PathLink> pathLinks;

    push::init(bytes);

    // jump to the main path, wherever that may end up in memory
    push::jump(bytes, graph.start, pathLinks);

    for (auto& entry : graph.map) {
        auto& path = entry.second;
        path->startIndex = bytes.size();
        generateOpt(*path, staticBindings, bytes, pathLinks);
    }

    for (const auto& pathLink : pathLinks) {
        const auto offset = pathLink.next->startIndex - pathLink.patchIndex - 4;
        writeOffset32(bytes, pathLink.patchIndex, offset);
    }
}