#ifndef STATICBINDINGS_H
#define STATICBINDINGS_H

#include <cstdint>


typedef void (*FunV1)(int64_t); // writeInt64, writeChar

typedef int64_t (*Fun10)(); // readInt64, readChar, rand4
typedef int64_t (*Fun11)(int64_t);
typedef int64_t (*Fun12)(int64_t, int64_t); // get
typedef int64_t (*Fun14)(int64_t, int64_t, int64_t, ino64_t); // put


struct StaticBindings {
    int64_t* stash;
    Fun12 get;
    Fun14 put;
    Fun10 readInt64;
    Fun10 readChar;
    Fun11 writeInt64;
    Fun11 writeChar;
    Fun10 rand4;
};


#endif //STATICBINDINGS_H
