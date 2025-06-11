#ifndef GENERATEOPT_H
#define GENERATEOPT_H

#include <variant>

#include "StaticBindings.h"
#include "Graph.h"


enum class InstrType : uint8_t {
    Push,
    Add, Sub, SubRev, Mul, Mul1, Div, Mod,
    Not, Gt, Gte, Lt, Lte,
    Dup, Swap, Drop,
    Get, Get2, Put, Put2,
    ReadInt64, ReadChar,
    WriteInt64, WriteChar,
    If, Rand, End
};

struct Push { int64_t value; };
struct Add {};
struct Sub {};
struct SubRev {};
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

typedef std::variant<
    Push,
    Add, Sub, SubRev, Mul, Mul1, Div, Mod,
    Not, Gt, Gte, Lt, Lte,
    Dup, Swap, Drop,
    Get, Get2, Put, Put2,
    ReadInt64, ReadChar,
    WriteInt64, WriteChar,
    If, Rand, End
> Instr;

std::string stringify (const Instr&);

void optimize (const std::vector<PathletEntry>& entries, std::vector<Instr>& finals);

void generateOpt (const Path&, const StaticBindings&, const Boolfield&, std::vector<uint8_t>&, std::vector<PathLink>&);

void generateOpt (Graph&, const StaticBindings&, const Boolfield&, std::vector<uint8_t>&);


#endif //GENERATEOPT_H