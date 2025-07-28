#include "CompType.h"
#include "generate.h"
#include "Pathlet.h"
#include "push.h"

#include "generateOpt.h"


struct InstrStringifier {
    std::string operator() (const Push& push) const { return std::string { "Push<" } + std::to_string(push.value) + ">"; }
    std::string operator() (const Add&) const { return std::string { "Add" }; }
    std::string operator() (const Add1& add1) const { return std::string { "Add1<" } + std::to_string(add1.value) + ">"; }
    std::string operator() (const Sub&) const { return std::string { "Sub" }; }
    std::string operator() (const SubRev&) const { return std::string { "SubRev" }; }
    std::string operator() (const Mul&) const { return std::string { "Mul" }; }
    std::string operator() (const Mul1& mul1) const { return std::string { "Mul1<" } + std::to_string(mul1.value) + ">"; }
    std::string operator() (const Sqr&) const { return std::string { "Sqr" }; }
    std::string operator() (const Div&) const { return std::string { "Div" }; }
    std::string operator() (const Div1& div1) const { return std::string { "Div1<" + std::to_string(div1.value) + ">" }; }
    std::string operator() (const Mod&) const { return std::string { "Mod" }; }
    std::string operator() (const Mod1& mod1) const { return std::string { "Mod1<" + std::to_string(mod1.value) + ">" }; }
    std::string operator() (const Not&) const { return std::string { "Not" }; }
    std::string operator() (const Comp& comp) const { return std::string { std::string { "Comp<" } + stringify(comp.type) + ">" }; }
    std::string operator() (const Dup&) const { return std::string { "Dup" }; }
    std::string operator() (const Swap&) const { return std::string { "Swap" }; }
    std::string operator() (const Drop&) const { return std::string { "Drop" }; }
    std::string operator() (const Get&) const { return std::string { "Get" }; }
    std::string operator() (const Get2& get2) const { return std::string { "Get2<" } + std::to_string(get2.x) + ", " + std::to_string(get2.y) + ">"; }
    std::string operator() (const Put&) const { return std::string { "Put" }; }
    std::string operator() (const Put2& put2) const { return std::string { "Put2<" } + std::to_string(put2.x) + ", " + std::to_string(put2.y) + ">"; }
    std::string operator() (const ReadInt64&) const { return std::string { "ReadInt64" }; }
    std::string operator() (const ReadChar&) const { return std::string { "ReadChar" }; }
    std::string operator() (const WriteInt64&) const { return std::string { "WriteInt64" }; }
    std::string operator() (const WriteChar&) const { return std::string { "WriteChar" }; }
    std::string operator() (const If&) const { return std::string { "If" }; }
    std::string operator() (const NotIf& notIf) const { return std::string { "NotIf<" } + std::to_string(notIf.dup) + ">"; }
    std::string operator() (const CompIf& compIf) const { return std::string { "CompIf<" } + stringify(compIf.type) + ">"; }
    std::string operator() (const Comp1If& comp1If) const { return std::string { "Comp1If<" } + stringify(comp1If.type) + ", " + std::to_string(comp1If.value) + ", " + std::to_string(comp1If.dup) + ">"; }
    std::string operator() (const Rand&) const { return std::string { "Rand" }; }
    std::string operator() (const End&) const { return std::string { "End" }; }
    std::string operator() (const ChainStart&) const { return std::string { "[" }; }
    std::string operator() (const ChainEnd&) const { return std::string { "]" }; }
};

std::string stringify (const Instr& instr) {
    return std::visit(InstrStringifier {}, instr);
}

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
            case '`': next.emplace_back(Comp { CompType::Gt }); break;
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

bool matchesUnsafe (const std::vector<Instr>& instructions, InstrType i0) {
    return instructions.end()[-1].index() == static_cast<size_t>(i0);
}

bool matchesUnsafe (const std::vector<Instr>& instructions, InstrType i0, InstrType i1) {
    return instructions.end()[-2].index() == static_cast<size_t>(i0) &&
        instructions.end()[-1].index() == static_cast<size_t>(i1);
}

bool matchesUnsafe (const std::vector<Instr>& instructions, InstrType i0, InstrType i1, InstrType i2) {
    return instructions.end()[-3].index() == static_cast<size_t>(i0) &&
        instructions.end()[-2].index() == static_cast<size_t>(i1) &&
        instructions.end()[-1].index() == static_cast<size_t>(i2);
}

bool matchesUnsafe (const std::vector<Instr>& instructions, size_t index, InstrType i0) {
    return instructions[index + 0].index() == static_cast<size_t>(i0);
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

InstrType getType (Instr instr) {
    return static_cast<InstrType>(instr.index());
}

int64_t getPushValue (Instr push) {
    return std::get<Push>(push).value;
}

void setPushValue (Instr& push, int64_t value) {
    std::get<Push>(push).value = value;
}

CompType getCompType (Instr comp) {
    return std::get<Comp>(comp).type;
}

void stackPass (const std::vector<Instr>& prev, std::vector<Instr>& next) {
    for (const auto& instr : prev) {
        const auto instrType = getType(instr);
        const auto stackSize = next.size();

        if (stackSize >= 3) {
            switch (instrType) {
                case InstrType::Add:
                    // a+b+
                    // (... + a) + b
                    // ... + (a + b)
                    // (a+b)+
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Add, InstrType::Push)) {
                        const auto result = getPushValue(next.end()[-3]) + getPushValue(next.end()[-1]);
                        next.pop_back();
                        setPushValue(next.end()[-2], result);
                        continue;
                    }

                    // a-b+
                    // (... + -a) + b
                    // ... + (-a + b)
                    // (b-a)+
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Sub, InstrType::Push)) {
                        const auto result = getPushValue(next.end()[-1]) - getPushValue(next.end()[-3]);
                        next.pop_back();
                        setPushValue(next.end()[-2], result);
                        next.back() = Add {};
                        continue;
                    }

                    break;

                case InstrType::Sub:
                    // a+b-
                    // (... + a) - b
                    // ... + (a - b)
                    // (a-b)+
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Add, InstrType::Push)) {
                        const auto result = getPushValue(next.end()[-3]) - getPushValue(next.end()[-1]);
                        next.pop_back();
                        setPushValue(next.end()[-2], result);
                        continue;
                    }

                    // a-b-
                    // (... + -a) + -b
                    // ... + (-a + -b)
                    // ... - (a + b)
                    // (a+b)-
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Sub, InstrType::Push)) {
                        const auto result = getPushValue(next.end()[-3]) - getPushValue(next.end()[-1]);
                        next.pop_back();
                        setPushValue(next.end()[-2], result);
                        continue;
                    }

                    break;

                case InstrType::Mul:
                    // a*b*
                    // (... * a) * b
                    // ... * (a * b)
                    // (a*b)*
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Mul, InstrType::Push)) {
                        const auto result = getPushValue(next.end()[-3]) * getPushValue(next.end()[-1]);
                        next.pop_back();
                        setPushValue(next.end()[-2], result);
                        continue;
                    }

                    break;

                default:; // nothing
            }
        }

        if (stackSize >= 2) {
            switch (instrType) {
                case InstrType::Add:
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Push)) {
                        const auto result = getPushValue(next.end()[-2]) + getPushValue(next.end()[-1]);
                        next.pop_back();
                        setPushValue(next.back(), result);
                        continue;
                    }

                    if (matchesUnsafe(next, InstrType::Push, InstrType::Get2)) {
                        std::iter_swap(next.end() - 2, next.end() - 1);
                        next.push_back(instr);
                        continue;
                    }

                    break;

                case InstrType::Sub:
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Push)) {
                        const auto result = getPushValue(next.end()[-2]) - getPushValue(next.end()[-1]);
                        next.pop_back();
                        setPushValue(next.back(), result);
                        continue;
                    }

                    if (matchesUnsafe(next, InstrType::Push, InstrType::Swap) && getPushValue(next.end()[-2]) == 0) {
                        setPushValue(next.end()[-2], -1);
                        next.back() = Mul {};
                        continue;
                    }

                    if (matchesUnsafe(next, InstrType::Push, InstrType::Get2)) {
                        std::iter_swap(next.end() - 2, next.end() - 1);
                        next.emplace_back(SubRev {});
                        continue;
                    }

                    break;

                case InstrType::Mul:
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Push)) {
                        const auto product = getPushValue(next.end()[-2]) * getPushValue(next.end()[-1]);
                        next.pop_back();
                        std::get<Push>(next.back()).value = product;
                        continue;
                    }

                    if (matchesUnsafe(next, InstrType::Push, InstrType::Get2)) {
                        std::iter_swap(next.end() - 2, next.end() - 1);
                        next.push_back(instr);
                        continue;
                    }

                    break;

                case InstrType::Swap:
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Push)) {
                        std::iter_swap(next.end() - 2, next.end() - 1);
                        continue;
                    }

                    break;

                case InstrType::Get:
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Push)) {
                        const auto x = getPushValue(next.end()[-2]);
                        const auto y = getPushValue(next.end()[-1]);
                        next.pop_back();
                        next.back() = Get2 { x, y };
                        continue;
                    }

                    break;

                case InstrType::Put:
                    if (matchesUnsafe(next, InstrType::Push, InstrType::Push)) {
                        const auto x = getPushValue(next.end()[-2]);
                        const auto y = getPushValue(next.end()[-1]);
                        next.pop_back();
                        next.back() = Put2 { x, y, std::get<Put>(instr).cursor };
                        continue;
                    }

                    break;

                default:; // nothing
            }
        }

        if (stackSize >= 1) {
            switch (instrType) {
                case InstrType::Add:
                    if (matchesUnsafe(next, InstrType::Dup)) {
                        next.end()[-1] = Push { 2 };
                        next.emplace_back(Mul {});
                        continue;
                    }
                    break;

                case InstrType::Dup:
                    if (matchesUnsafe(next, InstrType::Push)) {
                        next.push_back(next.back());
                        continue;
                    }
                    break;

                case InstrType::Swap:
                    if (matchesUnsafe(next, InstrType::Swap)) {
                        next.pop_back();
                        continue;
                    }
                    break;

                case InstrType::Drop:
                    if (matchesUnsafe(next, InstrType::Dup)) {
                        next.pop_back();
                        continue;
                    }
                    break;

                default:; // nothing
            }
        }

        next.push_back(instr);
    }
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
            if (matchesUnsafe(prev, index, InstrType::Swap, InstrType::Comp, InstrType::Not) && getCompType(prev[index + 1]) == CompType::Gt) {
                next.emplace_back(Comp { CompType::Gte });
                index += 3;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Push, InstrType::Get)) {
                next.emplace_back(Get2 { getPushValue(prev[index + 0]), getPushValue(prev[index + 1]) });
                index += 3;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Push, InstrType::Put)) {
                next.emplace_back(Put2 { getPushValue(prev[index + 0]), getPushValue(prev[index + 1]), std::get<Put>(prev[index + 2]).cursor });
                index += 3;
                continue;
            }
        }

        if (index < indexMaxM2) {
            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Add)) {
                next.emplace_back(Add1 { getPushValue(prev[index + 0]) });
                index += 2;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Sub)) {
                next.emplace_back(Add1 { -getPushValue(prev[index + 0]) });
                index += 2;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Mul)) {
                next.emplace_back(Mul1 { getPushValue(prev[index + 0]) });
                index += 2;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Div)) {
                next.emplace_back(Div1 { getPushValue(prev[index + 0]) });
                index += 2;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Push, InstrType::Mod)) {
                next.emplace_back(Mod1 { getPushValue(prev[index + 0]) });
                index += 2;
                continue;
            }

            // ab\`
            // b>a
            // a<b
            // ab(<)
            if (matchesUnsafe(prev, index, InstrType::Swap, InstrType::Comp) && getCompType(prev[index + 1]) == CompType::Gt) {
                next.emplace_back(Comp { CompType::Lt });
                index += 2;
                continue;
            }

            // ab`!
            // !(a>b)
            // a<=b
            // ab(<=)
            if (matchesUnsafe(prev, index, InstrType::Comp, InstrType::Not) && getCompType(prev[index + 0]) == CompType::Gt) {
                next.emplace_back(Comp { CompType::Lte });
                index += 2;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Swap, InstrType::Sub)) {
                next.emplace_back(SubRev {});
                index += 2;
                continue;
            }

            if (matchesUnsafe(prev, index, InstrType::Dup, InstrType::Mul)) {
                next.emplace_back(Sqr {});
                index += 2;
                continue;
            }
        }

        next.push_back(prev[index]);
        index++;
    }
}

bool isChainable (const Instr& instr) {
    const auto type = static_cast<InstrType>(instr.index());

    return type == InstrType::Add ||
        type == InstrType::Add1 ||
        type == InstrType::Sub ||
        type == InstrType::SubRev ||
        type == InstrType::Mul ||
        type == InstrType::Mul1 ||
        type == InstrType::Sqr;
}

void chainPass (const std::vector<Instr>& prev, std::vector<Instr>& next) {
    const auto indexMaxM0 = prev.size();
    const auto indexMaxM1 = indexMaxM0 - 1;

    size_t index = 0;
    bool chainMode = false;

    while (index < indexMaxM1) {
        if (chainMode) {
            if (!isChainable(prev[index])) {
                next.emplace_back(ChainEnd {});
                chainMode = false;
            }
            next.push_back(prev[index]);
            index++;
            continue;
        }

        if (isChainable(prev[index]) && isChainable(prev[index + 1])) {
            next.emplace_back(ChainStart {});
            chainMode = true;
        }

        next.push_back(prev[index]);

        index++;
    }

    if (chainMode) {
        if (isChainable(prev[index])) {
            next.push_back(prev[index]);
            next.emplace_back(ChainEnd {});
        } else {
            next.emplace_back(ChainEnd {});
            next.push_back(prev[index]);
        }
    } else {
        next.push_back(prev[index]);
    }
}

void ifPass (std::vector<Instr>& cur) {
    if (cur.empty()) [[unlikely]] {
        return;
    }

    const auto indexMaxM0 = cur.size();
    const auto indexMaxM1 = indexMaxM0 - 1;
    const auto indexMaxM2 = indexMaxM0 - 2;
    const auto indexMaxM3 = indexMaxM0 - 3;
    const auto indexMaxM4 = indexMaxM0 - 4;

    if (!matchesUnsafe(cur, indexMaxM1, InstrType::If)) {
        return;
    }

    if (matchesUnsafe(cur, indexMaxM2, InstrType::Comp)) {
        const auto compType = getCompType(cur[indexMaxM2]);

        if (matchesUnsafe(cur, indexMaxM3, InstrType::Push)) {
            const auto value = getPushValue(cur[indexMaxM3]);

            if (matchesUnsafe(cur, indexMaxM4, InstrType::Dup)) {
                cur.erase(cur.end() - 3, cur.end()); // faster than resize ?? // this not really needed - codegen stops at if
                cur[indexMaxM4] = Comp1If { compType, value, true };
                return;
            }

            cur.erase(cur.end() - 2, cur.end());
            cur[indexMaxM3] = Comp1If { compType, value, false };
            return;
        }

        cur.pop_back();
        cur[indexMaxM2] = CompIf { compType };
        return;
    }

    if (matchesUnsafe(cur, indexMaxM2, InstrType::Not)) {
        if (matchesUnsafe(cur, indexMaxM3, InstrType::Dup)) {
            cur.erase(cur.end() - 2, cur.end());
            cur[indexMaxM3] = NotIf { true };
            return;
        }

        cur.pop_back();
        cur[indexMaxM2] = NotIf { false };
        return;
    }
}

void optimize (const std::vector<PathletEntry>& entries, std::vector<Instr>& postFinals) {
    std::vector<Instr> firsts;
    translatePass(entries, firsts);

    std::vector<Instr> seconds;
    stackPass(firsts, seconds);

    std::vector<Instr> finals;
    finalPass(seconds, finals);
    chainPass(finals, postFinals);
    ifPass(postFinals);
}

void generateOpt (
    const Path& path,
    const StaticBindings& staticBindings,
    const Boolfield& boolfield,
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

    std::vector<Instr> finals;
    optimize(path.entries, finals);

    bool isChainMode = false;
    auto chainSize = -1;

    for (const auto& instr : finals) {
        const auto type = static_cast<InstrType>(instr.index());

        if (isChainMode) {
            if (type == InstrType::ChainEnd) {
                push::chain::chainEnd(bytes, chainSize - 2);
                isChainMode = false;
                continue;
            }

            switch (type) {
                case InstrType::Add: push::chain::add(bytes, chainSize); chainSize++; break;
                case InstrType::Add1: push::chain::add1(bytes, std::get<Add1>(instr).value); break;
                case InstrType::Sub: push::chain::sub(bytes, chainSize); chainSize++; break;
                case InstrType::SubRev: push::chain::subRev(bytes, chainSize); chainSize++; break;
                case InstrType::Mul: push::chain::mul(bytes, chainSize); chainSize++; break;
                case InstrType::Mul1: push::chain::mul1(bytes, std::get<Mul1>(instr).value); break;
                case InstrType::Sqr: push::chain::sqr(bytes); break;
                default: break;
            }

            continue;
        }

        if (type == InstrType::ChainStart) {
            isChainMode = true;
            chainSize = 2;
            push::chain::chainStart(bytes);
            continue;
        }

        switch (type) {
            case InstrType::Push: push::value(bytes, std::get<Push>(instr).value); break;

            case InstrType::Add: push::add(bytes); break;
            case InstrType::Add1: push::add1(bytes, std::get<Add1>(instr).value); break;
            case InstrType::Sub: push::sub(bytes); break;
            case InstrType::SubRev: push::subRev(bytes); break;
            case InstrType::Mul: push::mul(bytes); break;
            case InstrType::Mul1: push::mul1(bytes, std::get<Mul1>(instr).value); break;
            case InstrType::Sqr: push::sqr(bytes); break;
            case InstrType::Div: push::div(bytes); break;
            case InstrType::Div1: push::div1(bytes, std::get<Div1>(instr).value); break;
            case InstrType::Mod: push::mod(bytes); break;
            case InstrType::Mod1: push::mod1(bytes, std::get<Mod1>(instr).value); break;

            case InstrType::Not: push::not_(bytes); break;
            case InstrType::Comp: push::comp(bytes, std::get<Comp>(instr).type); break;

            case InstrType::Dup: push::dup(bytes); break;
            case InstrType::Swap: push::swap(bytes); break;
            case InstrType::Drop: push::drop(bytes); break;

            case InstrType::Get: push::get(bytes, staticBindings.stash, staticBindings.get); break;
            case InstrType::Get2: {
                const auto [x, y] = std::get<Get2>(instr);

                if (Playfield::isWithinBounds(x, y)) {
                    push::get2(bytes, staticBindings.playfieldData, y * Playfield::width + x);
                } else {
                    push::value(bytes, 0);
                }

                break;
            }

            case InstrType::Put: push::put(bytes, staticBindings.stash, staticBindings.put, std::get<Put>(instr).cursor); break;
            case InstrType::Put2: {
                const auto [x, y, cursor] = std::get<Put2>(instr);

                if (Playfield::isWithinBounds(x, y)) {
                    if (boolfield.getAtUnsafe(x, y)) {
                        push::put2Recomp(bytes, staticBindings.stash, staticBindings.put, x, y, cursor);
                    } else {
                        push::put2NoRecomp(bytes, staticBindings.playfieldData, y * Playfield::width + x);
                    }
                } else {
                    push::drop(bytes);
                }

                break;
            }

            case InstrType::ReadInt64: push::read(bytes, staticBindings.stash, staticBindings.readInt64); break;
            case InstrType::ReadChar: push::read(bytes, staticBindings.stash, staticBindings.readChar); break;
            case InstrType::WriteInt64: push::write(bytes, staticBindings.stash, staticBindings.writeInt64); break;
            case InstrType::WriteChar: push::write(bytes, staticBindings.stash, staticBindings.writeChar); break;

            case InstrType::If: push::if_(bytes, path, pathLinks); return;
            case InstrType::Rand: push::rand(bytes, staticBindings.stash, staticBindings.rand4, path, pathLinks); return;
            case InstrType::End: push::end(bytes, staticBindings.stash); return;

            case InstrType::NotIf: push::notIf(bytes, std::get<NotIf>(instr).dup, path, pathLinks); break;
            case InstrType::CompIf: push::compIf(bytes, std::get<CompIf>(instr).type, path, pathLinks); break;
            case InstrType::Comp1If: {
                const auto [type, value, dup] = std::get<Comp1If>(instr);
                push::comp1If(bytes, type, value, dup, path, pathLinks);
                return;
            }

            default: break;
        }
    }
}

void generateOpt (const Graph& graph, const StaticBindings& staticBindings, const Boolfield& boolfield, std::vector<uint8_t>& bytes) {
    std::vector<PathLink> pathLinks;

    push::init(bytes);

    // jump to the main path, wherever that may end up in memory
    push::jump(bytes, graph.start, pathLinks);

    for (auto& entry : graph.map) {
        auto& path = entry.second;
        path->startIndex = bytes.size();
        generateOpt(*path, staticBindings, boolfield, bytes, pathLinks);
    }

    for (const auto& pathLink : pathLinks) {
        const auto offset = pathLink.next->startIndex - pathLink.patchIndex - 4;
        writeOffset32(bytes, pathLink.patchIndex, offset);
    }
}