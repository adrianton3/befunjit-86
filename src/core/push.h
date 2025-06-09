#ifndef PUSH_H
#define PUSH_H

#include <cstdint>
#include <vector>

#include "Cursor.h"
#include "Path.h"
#include "StaticBindings.h"


template <uint64_t N>
uint8_t getByte (uint64_t v) {
    return (v >> (N * 8)) & 0xff;
}

namespace push {

    void init (std::vector<uint8_t>& bytes);

    void value (std::vector<uint8_t>& bytes, int64_t value);

    void add (std::vector<uint8_t>& bytes);

    void sub (std::vector<uint8_t>& bytes);

    void subRev (std::vector<uint8_t>& bytes);

    void mul (std::vector<uint8_t>& bytes);

    void mul1 (std::vector<uint8_t>& bytes, int64_t value);

    void div (std::vector<uint8_t>& bytes);

    void mod (std::vector<uint8_t>& bytes);

    void not_ (std::vector<uint8_t>& bytes);

    void gt (std::vector<uint8_t>& bytes);

    void gte (std::vector<uint8_t>& bytes);

    void lt (std::vector<uint8_t>& bytes);

    void lte (std::vector<uint8_t>& bytes);

    void dup (std::vector<uint8_t>& bytes);

    void swap (std::vector<uint8_t>& bytes);

    void drop (std::vector<uint8_t>& bytes);

    void get (std::vector<uint8_t>& bytes, int64_t const* stash, Fun12 get);

    void get2 (std::vector<uint8_t>& bytes, int64_t const* playfieldData, uint64_t indexFlat);

    void put (std::vector<uint8_t>& bytes, int64_t const* stash, Fun14 put, const Cursor& cursor); // FunV4

    void put2Recomp (std::vector<uint8_t>& bytes, int64_t const* stash, Fun14 put, uint8_t x, uint8_t y, const Cursor& cursor); // FunV4

    void put2NoRecomp (std::vector<uint8_t>& bytes, int64_t const* playfieldData, uint64_t indexFlat);

    void read (std::vector<uint8_t>& bytes, int64_t const* stash, Fun10 read);

    void write (std::vector<uint8_t>& bytes, int64_t const* stash, Fun11 write);

    void if_ (std::vector<uint8_t>& bytes, const Path& path, std::vector<PathLink>& pathLinks);

    void rand (std::vector<uint8_t>& bytes, int64_t const* stash, Fun10 rand4, const Path& path, std::vector<PathLink>& pathLinks);

    void end (std::vector<uint8_t>& bytes, int64_t const* stash);

    void jump (std::vector<uint8_t>& bytes, Path* path, std::vector<PathLink>& pathLinks);

    void loopback (std::vector<uint8_t>& bytes, int64_t loopbackIndex);

}

void writeOffset32 (std::vector<uint8_t>& bytes, uint64_t index, int32_t offset);

#endif //PUSH_H
