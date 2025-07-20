#ifndef GENERATEOPT_H
#define GENERATEOPT_H

#include <variant>

#include "CompType.h"
#include "StaticBindings.h"
#include "Graph.h"


enum class InstrType : uint8_t {
    Push,
    Add, Add1, Sub, SubRev, Mul, Mul1, Sqr, Div, Mod,
    Not, Comp,
    Dup, Swap, Drop,
    Get, Get2, Put, Put2,
    ReadInt64, ReadChar,
    WriteInt64, WriteChar,
    If, Rand, End,
    CompIf, Comp1If,
    ChainStart, ChainEnd
};

struct Push { int64_t value; };
struct Add {};
struct Add1 { int64_t value; };
struct Sub {};
struct SubRev {};
struct Mul {};
struct Mul1 { int64_t value; };
struct Sqr {};
struct Div {};
struct Mod {};

struct Not {};
struct Comp { CompType type; };

struct Dup {};
struct Swap {};
struct Drop {};

struct Get {};
struct Get2 { int64_t x; int64_t y; };
struct Put { Cursor cursor; };
struct Put2 { int64_t x; int64_t y; Cursor cursor; };

struct ReadInt64 {};
struct ReadChar {};
struct WriteInt64 {};
struct WriteChar {};

struct If {};
struct Rand {};
struct End {};

struct CompIf { CompType type; };
struct Comp1If { CompType type; int64_t value; bool dup; };

struct ChainStart {};
struct ChainEnd {};

typedef std::variant<
    Push,
    Add, Add1, Sub, SubRev, Mul, Mul1, Sqr, Div, Mod,
    Not, Comp,
    Dup, Swap, Drop,
    Get, Get2, Put, Put2,
    ReadInt64, ReadChar,
    WriteInt64, WriteChar,
    If, Rand, End,
    CompIf, Comp1If,
    ChainStart, ChainEnd
> Instr;

std::string stringify (const Instr&);

void optimize (const std::vector<PathletEntry>&, std::vector<Instr>&);

void generateOpt (const Path&, const StaticBindings&, const Boolfield&, std::vector<uint8_t>&, std::vector<PathLink>&);

void generateOpt (const Graph&, const StaticBindings&, const Boolfield&, std::vector<uint8_t>&);


#endif //GENERATEOPT_H