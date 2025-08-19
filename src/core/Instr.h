#ifndef INSTR_H
#define INSTR_H

#include <cstdint>
#include <string>
#include <variant>

#include "CompType.h"
#include "Cursor.h"


enum class InstrType : uint8_t {
    Push,
    Add, Add1, Sub, SubRev, Mul, Mul1, Sqr, Div, Div1, Mod, Mod1,
    Not, Comp,
    Dup, Swap, Drop,
    Get, Get2, Put, Put2,
    ReadInt64, ReadChar,
    WriteInt64, WriteChar,
    If, Rand, End,
    NotIf, CompIf, Comp1If,
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
struct Div1 { int64_t value; };
struct Mod {};
struct Mod1 { int64_t value; };

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

struct NotIf { bool dup; };
struct CompIf { CompType type; };
struct Comp1If { CompType type; int64_t value; bool dup; };

struct ChainStart {};
struct ChainEnd {};

typedef std::variant<
    Push,
    Add, Add1, Sub, SubRev, Mul, Mul1, Sqr, Div, Div1, Mod, Mod1,
    Not, Comp,
    Dup, Swap, Drop,
    Get, Get2, Put, Put2,
    ReadInt64, ReadChar,
    WriteInt64, WriteChar,
    If, Rand, End,
    NotIf, CompIf, Comp1If,
    ChainStart, ChainEnd
> Instr;

std::string stringify (const Instr&);


#endif //INSTR_H
