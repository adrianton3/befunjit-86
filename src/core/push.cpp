#include <bit>
#include <utility>

#include "Path.h"

#include "push.h"


void push::init (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x51                                       // push rcx so the stack is aligned by 16 bytes on next call
                                                   // remember to pop it back on every end
    });
}

void push::value (std::vector<uint8_t>& bytes, int64_t value) {
    if (std::in_range<std::int32_t>(value)) [[likely]] {
        bytes.insert(bytes.end(), {
            0x48, 0xc7, 0x04, 0xf7, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), // mov [rdi + rsi * 8], 32bit
        });
    } else {
        bytes.insert(bytes.end(), {
            0x48, 0xb8, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), getByte<4>(value), getByte<5>(value), getByte<6>(value), getByte<7>(value), // mov rax, 64bit
            0x48, 0x89, 0x04, 0xf7,                // mov [rdi + rsi * 8], rax
        });
    }

    bytes.insert(bytes.end(), {
        0x48, 0xff, 0xc6                           // inc rsi
    });
}

void push::add (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x01, 0x44, 0xf7, 0xf0,              // add [rdi + rsi * 8 - 16], rax
        0x48, 0xff, 0xce                           // dec rsi
    });
}

void push::add1 (std::vector<uint8_t>& bytes, int64_t value) {
    if (value == -1) {
        bytes.insert(bytes.end(), {
            0x48, 0xff, 0x4c, 0xf7, 0xf8           // dec [rdi + rsi * 8 - 8]
        });
        return;
    }

    if (value == 0) {
        // nothing
        return;
    }

    if (value == 1) {
        bytes.insert(bytes.end(), {
            0x48, 0xff, 0x44, 0xf7, 0xf8           // inc [rdi + rsi * 8 - 8]
        });
        return;
    }

    if (std::in_range<std::int8_t>(value)) {
        bytes.insert(bytes.end(), {
            0x48, 0x83, 0x44, 0xf7, 0xf8, getByte<0>(value) // add [rdi + rsi * 8 - 8], 8bit
        });
        return;
    }

    if (std::in_range<std::int32_t>(value)) {
        bytes.insert(bytes.end(), {
            0x48, 0x81, 0x44, 0xf7, 0xf8, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value) // add [rdi + rsi * 8 - 8], 32bit
        });
        return;
    }

    bytes.insert(bytes.end(), {
        0x48, 0xb8, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), getByte<4>(value), getByte<5>(value), getByte<6>(value), getByte<7>(value), // mov rax, 64bit
        0x48, 0x01, 0x44, 0xf7, 0xf8               // add [rdi + rsi * 8 - 8], rax
    });
}

void push::sub (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x29, 0x44, 0xf7, 0xf0,              // sub [rdi + rsi * 8 - 16], rax
        0x48, 0xff, 0xce                           // dec rsi
    });
}

void push::subRev (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x2b, 0x44, 0xf7, 0xf0,              // sub rax, [rdi + rsi * 8 - 16]
        0x48, 0x89, 0x44, 0xf7, 0xf0,              // mov [rdi + rsi * 8 - 16], rax
        0x48, 0xff, 0xce                           // dec rsi
    });
}

void push::mul (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x0f, 0xaf, 0x44, 0xf7, 0xf0,        // imul rax, [rdi + rsi * 8 - 16]
        0x48, 0x89, 0x44, 0xf7, 0xf0,              // mov [rdi + rsi * 8 - 16], rax
        0x48, 0xff, 0xce                           // dec rsi
    });
}

void push::mul1 (std::vector<uint8_t>& bytes, int64_t value) {
    if (value == -1) {
        bytes.insert(bytes.end(), {
            0x48, 0xf7, 0x5c, 0xf7, 0xf8           // neg [rdi + rsi * 8 - 8]
        });
        return;
    }

    if (value == 0) {
        // keep this case separate since the pot test accepts 0
        bytes.insert(bytes.end(), {
            0x48, 0xc7, 0x44, 0xf7, 0xf8, 0x00, 0x00, 0x00, 0x00 // mov [rdi + rsi * 8 - 8], 0
        });
        return;
    }

    if (value == 1) {
        // nothing
        return;
    }

    if (value == 2) {
        bytes.insert(bytes.end(), {
            0x48, 0xd1, 0x64, 0xf7, 0xf8           // shl
        });
        return;
    }

    if (value == 3) {
        bytes.insert(bytes.end(), {
            0x48, 0x8b, 0x44, 0xf7, 0xf8,          // mov rax, [rdi + rsi * 8 - 8]
            0x48, 0x8d, 0x04, 0x40,                // lea rax, [rax + rax * 2]
            0x48, 0x89, 0x44, 0xf7, 0xf8,          // mov [rdi + rsi * 8 - 8], rax
        });
        return;
    }

    if (value == 5) {
        bytes.insert(bytes.end(), {
            0x48, 0x8b, 0x44, 0xf7, 0xf8,          // mov rax, [rdi + rsi * 8 - 8]
            0x48, 0x8d, 0x04, 0x80,                // lea rax, [rax + rax * 4]
            0x48, 0x89, 0x44, 0xf7, 0xf8,          // mov [rdi + rsi * 8 - 8], rax
        });
        return;
    }

    if ((value & (value - 1)) == 0) {
        // powers of two
        const auto shift = std::bit_width(static_cast<uint64_t>(value)) - 1;
        bytes.insert(bytes.end(), {
            0x48, 0xc1, 0x64, 0xf7, 0xf8, static_cast<uint8_t>(shift) // shl [rdi + rsi * 8 - 8], log2(value)
        });
        return;
    }

    if (std::in_range<std::int8_t>(value)) [[likely]] {
        bytes.insert(bytes.end(), {
            0x48, 0x6b, 0x54, 0xf7, 0xf8, getByte<0>(value), // imul rdx, [rdi + rsi * 8 - 8], 8bit
            0x48, 0x89, 0x54, 0xf7, 0xf8           // mov [rdi + rsi * 8 - 8], rdx
        });
        return;
    }

    if (std::in_range<std::int32_t>(value)) {
        bytes.insert(bytes.end(), {
            0x48, 0x69, 0x54, 0xf7, 0xf8, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), // imul, rdx, [rdi + rsi * 8 + 8], 32bit
            0x48, 0x89, 0x54, 0xf7, 0xf8,          // mov [rdi + rsi * 8 - 8], rdx
        });
        return;
    }

    // it would take quite a few fold passes to accumulate such a large number in a cell
    // but this bit of code doesn't know that
    bytes.insert(bytes.end(), {
        0x48, 0xb8, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), getByte<4>(value), getByte<5>(value), getByte<6>(value), getByte<7>(value),  // mov rax, 64bit
        0x48, 0x0f, 0xaf, 0x44, 0xf7, 0xf8,        // imul rax, [rdi + rsi * 8 - 8]
        0x48, 0x89, 0x44, 0xf7, 0xf8,              // mov [rdi + rsi * 8 - 8], rax
    });
}

void push::sqr (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x0f, 0xaf, 0xc0,                    // imul rax, rax
        0x48, 0x89, 0x44, 0xf7, 0xf8,              // mov [rdi + rsi * 8 - 8], rax
    });
}

void push::div (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf0,              // mov rax, [rdi + rsi * 8 - 16]
        0x48, 0x99,                                // cqo
        0x48, 0xf7, 0x7c, 0xf7, 0xf8,              // idiv qword [rdi + rsi * 8 - 8]
        0x48, 0x89, 0x44, 0xf7, 0xf0,              // mov [rdi + rsi * 8 - 16], rax
        0x48, 0xff, 0xce                           // dec rsi
    });
}

void push::div1 (std::vector<uint8_t>& bytes, int64_t value) {
    if (value == -1) {
        bytes.insert(bytes.end(), {
            0x48, 0xf7, 0x5c, 0xf7, 0xf8           // neg [rdi + rsi * 8 - 8]
        });
        return;
    }

    if (value == 1) {
        // nothing
        return;
    }

    if (value == 2) {
        bytes.insert(bytes.end(), {
            0x48, 0x8b, 0x44, 0xf7, 0xf8,          // mov rax, [rdi + rsi * 8 - 8]
            0x48, 0x89, 0xc1,                      // mov rcx, rax
            0x48, 0xc1, 0xe9, 0x3f,                // shr rcx, 63
            0x48, 0x01, 0xc1,                      // add rcx, rax
            0x48, 0xd1, 0xf9,                      // sar rcx
            0x48, 0x89, 0x4c, 0xf7, 0xf8           // mov [rdi + rsi * 8 - 8], rcx
        });
        return;
    }

    if (value > 0 && (value & (value - 1)) == 0) {
        // powers of two
        const auto shift = std::bit_width(static_cast<uint64_t>(value)) - 1;
        const auto valueM1 = value - 1;
        bytes.insert(bytes.end(), {
            0x48, 0x8b, 0x44, 0xf7, 0xf8,          // mov rax, [rdi + rsi * 8 - 8]
            0x48, 0x8d, 0x88, getByte<0>(valueM1), getByte<1>(valueM1), getByte<2>(valueM1), getByte<3>(valueM1), // lea rcx, [rax + (value-1)]
            0x48, 0x85, 0xc0,                      // test rax, rax
            0x48, 0x0f, 0x49, 0xc8,                // cmovns rcx, rax
            0x48, 0xc1, 0xf9, getByte<0>(shift),   // sar rcx, shift
            0x48, 0x89, 0x4c, 0xf7, 0xf8           // mov [rdi + rsi * 8 - 8], rcx
        });
        return;
    }

    bytes.insert(bytes.end(), {
        0x48, 0xb9, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), getByte<4>(value), getByte<5>(value), getByte<6>(value), getByte<7>(value), // mov rcx, 64bit
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x99,                                // cqo
        0x48, 0xf7, 0xf9,                          // idiv rcx
        0x48, 0x89, 0x44, 0xf7, 0xf8,              // mov [rdi + rsi * 8 - 8], rax
    });
}

void push::mod (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf0,              // mov rax, [rdi + rsi * 8 - 16]
        0x48, 0x99,                                // cqo
        0x48, 0xf7, 0x7c, 0xf7, 0xf8,              // idiv qword [rdi + rsi * 8 - 8]
        0x48, 0x89, 0x54, 0xf7, 0xf0,              // mov [rdi + rsi * 8 - 16], rdx
        0x48, 0xff, 0xce                           // dec rsi
    });
}

void push::mod1 (std::vector<uint8_t>& bytes, int64_t value) {
    if (value == 1) {
        bytes.insert(bytes.end(), {
            0x48, 0xc7, 0x44, 0xf7, 0xf8, 0x00, 0x00, 0x00, 0x00 // mov [rdi + rsi * 8 - 8], 0
        });
        return;
    }

    if (value == 2) {
        bytes.insert(bytes.end(), {
            0x48, 0x8b, 0x44, 0xf7, 0xf8,          // mov rax, [rdi + rsi * 8 - 8]
            0x48, 0x89, 0xc1,                      // mov rcx, rax
            0x48, 0xc1, 0xe9, 0x3f,                // shr rcx, 63
            0x48, 0x01, 0xc1,                      // add rcx, rax
            0x48, 0x83, 0xe1, 0xfe,                // and rcx, 0b111...1110
            0x48, 0x29, 0xc8,                      // sub rax, rcx
            0x48, 0x89, 0x44, 0xf7, 0xf8           // mov [rdi + rsi * 8 - 8], rax
        });
        return;
    }

    if (value > 0 && (value & (value - 1)) == 0) {
        // powers of two
        const auto valueM1 = value - 1;
        const auto valueI = -value;
        bytes.insert(bytes.end(), {
            0x48, 0x8b, 0x44, 0xf7, 0xf8,          // mov rax, [rdi + rsi * 8 - 8]
            0x48, 0x8d, 0x88, getByte<0>(valueM1), getByte<1>(valueM1), getByte<2>(valueM1), getByte<3>(valueM1), // lea rcx, [rax + (value-1)]
            0x48, 0x85, 0xc0,                      // test rax, rax
            0x48, 0x0f, 0x49, 0xc8,                // cmovns rcx, rax
            0x48, 0x81, 0xe1, getByte<0>(valueI), getByte<1>(valueI), getByte<2>(valueI), getByte<3>(valueI), // and rcx, -value
            0x48, 0x29, 0xc8,                      // sub rax, rcx
            0x48, 0x89, 0x44, 0xf7, 0xf8           // mov [rdi + rsi * 8 - 8], rax
        });
        return;
    }

    bytes.insert(bytes.end(), {
        0x48, 0xb9, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), getByte<4>(value), getByte<5>(value), getByte<6>(value), getByte<7>(value), // mov rcx, 64bit
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x99,                                // cqo
        0x48, 0xf7, 0xf9,                          // idiv rcx
        0x48, 0x89, 0x54, 0xf7, 0xf8,              // mov [rdi + rsi * 8 - 8], rdx
    });
}

void push::not_ (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x4c, 0xf7, 0xf8,              // mov rcx, [rdi + rsi * 8 - 8]
        0x31, 0xc0,                                // xor eax, eax
        0x48, 0x85, 0xc9,                          // test rcx, rcx
        0x0f, 0x94, 0xc0,                          // sete al
        0x48, 0x89, 0x44, 0xf7, 0xf8               // mov qword [rdi + rsi * 8 - 8], rax
    });
}

void push::comp (std::vector<uint8_t>& bytes, CompType type) {
    const uint8_t compByte = type == CompType::Gt ? 0x9f
        : type == CompType::Gte ? 0x9d
        : type == CompType::Lt ? 0x9c
        : 0x9e;

    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x4c, 0xf7, 0xf8,              // mov rcx, [rdi + rsi * 8 - 8]
        0x48, 0x8b, 0x54, 0xf7, 0xf0,              // mov rdx, [rdi + rsi * 8 - 16]
        0x48, 0xff, 0xce,                          // dec rsi
        0x31, 0xc0,                                // xor eax, eax
        0x48, 0x39, 0xca,                          // cmp rdx, rcx
        0x0f, compByte, 0xc0,                      // setX al
        0x48, 0x89, 0x44, 0xf7, 0xf8               // mov qword [rdi + rsi * 8 - 8], rax
    });
}

void push::dup (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x89, 0x04, 0xf7,                    // mov [rdi + rsi * 8], rax
        0x48, 0xff, 0xc6                           // inc rsi
    });
}

void push::swap (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x8b, 0x4c, 0xf7, 0xf0,              // mov rcx, [rdi + rsi * 8 - 16]

        0x48, 0x89, 0x4c, 0xf7, 0xf8,              // mov [rdi + rsi * 8 - 8], rcx
        0x48, 0x89, 0x44, 0xf7, 0xf0,              // mov [rdi + rsi * 8 - 16], rax
    });
}

void push::drop (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0xff, 0xce                           // dec rsi
    });
}

void push::get (std::vector<uint8_t>& bytes, int64_t const* stash, Fun12 get) {
    const auto stashAddress = reinterpret_cast<uint64_t>(stash);
    const auto getAddress = reinterpret_cast<uint64_t>(get);

    bytes.insert(bytes.end(), {
        0x48, 0xff, 0xce,                          // dec rsi

        0x48, 0xb8, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rax, stashAddress
        0x48, 0x89, 0x38,                          // mov [rax], rdi
        0x48, 0x89, 0x70, 0x08,                    // mov [rax + 8], rsi

        0x4c, 0x8b, 0x04, 0xf7,                    // mov r8, [rdi + rsi * 8]
        0x4c, 0x8b, 0x4c, 0xf7, 0xf8,              // mov r9, [rdi + rsi * 8 - 8]

        0x4c, 0x89, 0xc6,                          // mov rsi, r8
        0x4c, 0x89, 0xcf,                          // mov rdi, r9

        0x48, 0xb8, getByte<0>(getAddress), getByte<1>(getAddress), getByte<2>(getAddress), getByte<3>(getAddress), getByte<4>(getAddress), getByte<5>(getAddress), getByte<6>(getAddress), getByte<7>(getAddress), // mov rax, getAddress
        0xff, 0xd0,                                // call rax

        0x48, 0xb9, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rcx, stashAddress
        0x48, 0x8b, 0x39,                          // mov rdi, [rcx]
        0x48, 0x8b, 0x71, 0x08,                    // mov rsi, [rcx + 8]

        0x48, 0x89, 0x44, 0xf7, 0xf8               // mov [rdi + rsi * 8 - 8], rax
    });
}

void push::get2 (std::vector<uint8_t>& bytes, int64_t const* playfieldData, uint64_t indexFlat) {
    const auto playfieldDataAddress = reinterpret_cast<uint64_t>(playfieldData);
    const auto offsetFlat = indexFlat * 8;

    bytes.insert(bytes.end(), {
        0x48, 0xb8, getByte<0>(playfieldDataAddress), getByte<1>(playfieldDataAddress), getByte<2>(playfieldDataAddress), getByte<3>(playfieldDataAddress), getByte<4>(playfieldDataAddress), getByte<5>(playfieldDataAddress), getByte<6>(playfieldDataAddress), getByte<7>(playfieldDataAddress), // mov rax, playfieldDataAddress
        0x48, 0x8b, 0x90, getByte<0>(offsetFlat), getByte<1>(offsetFlat), getByte<2>(offsetFlat), getByte<3>(offsetFlat), // mov rdx, [rax + offsetFlat]
        0x48, 0x89, 0x14, 0xf7,                    // mov [rdi + rsi * 8], rdx
        0x48, 0xff, 0xc6,                          // inc rsi
    });
}

void push::put (std::vector<uint8_t>& bytes, int64_t const* stash, Fun14 put, const Cursor& cursor) { // FunV4
    const auto stashAddress = reinterpret_cast<uint64_t>(stash);
    const auto putAddress = reinterpret_cast<uint64_t>(put);
    const auto cursorPacked = pack(cursor);

    bytes.insert(bytes.end(), {
        0x48, 0x83, 0xee, 0x03,                    // sub rsi, 3

        0x48, 0xb8, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rax, stashAddress
        0x48, 0x89, 0x38,                          // mov [rax], rdi
        0x48, 0x89, 0x70, 0x08,                    // mov [rax + 8], rsi

        0x48, 0x8b, 0x14, 0xf7,                    // mov rdx, [rdi + rsi * 8]
        0x4c, 0x8b, 0x44, 0xf7, 0x08,              // mov r8, [rdi + rsi * 8 + 8]
        0x4c, 0x8b, 0x4c, 0xf7, 0x10,              // mov r9, [rdi + rsi * 8 + 16]
        0x4c, 0x89, 0xc7,                          // mov rdi, r8
        0x4c, 0x89, 0xce,                          // mov rsi, r9
        0xb9, getByte<0>(cursorPacked), getByte<1>(cursorPacked), getByte<2>(cursorPacked), getByte<3>(cursorPacked), // mov rcx, cursorPacked

        0x48, 0xb8, getByte<0>(putAddress), getByte<1>(putAddress), getByte<2>(putAddress), getByte<3>(putAddress), getByte<4>(putAddress), getByte<5>(putAddress), getByte<6>(putAddress), getByte<7>(putAddress), // mov rax, putAddress
        0xff, 0xd0,                                // call rax

        0x48, 0x85, 0xc0,                          // test rax, rax
        0x74, 0x02,                                // jz 2
        0x59,                                      // pop rcx as a counter to the push in init
        0xc3,                                      // ret

        0x48, 0xb9, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rcx, stashAddress
        0x48, 0x8b, 0x39,                          // mov rdi, [rcx]
        0x48, 0x8b, 0x71, 0x08,                    // mov rsi, [rcx + 8]
    });
}

void push::put2Recomp (std::vector<uint8_t>& bytes, int64_t const* stash, Fun14 put, uint8_t x, uint8_t y, const Cursor& cursor) { // FunV4
    const auto stashAddress = reinterpret_cast<uint64_t>(stash);
    const auto putAddress = reinterpret_cast<uint64_t>(put);
    const auto cursorPacked = pack(cursor);

    bytes.insert(bytes.end(), {
        0x48, 0xff, 0xce,                          // dec rsi

        0x48, 0xb8, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rax, stashAddress
        0x48, 0x89, 0x38,                          // mov [rax], rdi
        0x48, 0x89, 0x70, 0x08,                    // mov [rax + 8], rsi

        0x48, 0x8b, 0x14, 0xf7,                    // mov rdx, [rdi + rsi * 8]
        0xb9, getByte<0>(cursorPacked), getByte<1>(cursorPacked), getByte<2>(cursorPacked), getByte<3>(cursorPacked), // mov rcx, cursor-packed

        0xbf, getByte<0>(x), 0x00, 0x00, 0x00,     // mov edi, x
        0xbe, getByte<0>(y), 0x00, 0x00, 0x00,     // mov esi, y

        0x48, 0xb8, getByte<0>(putAddress), getByte<1>(putAddress), getByte<2>(putAddress), getByte<3>(putAddress), getByte<4>(putAddress), getByte<5>(putAddress), getByte<6>(putAddress), getByte<7>(putAddress), // mov rax, putAddress
        0xff, 0xd0,                                // call rax

        0x59,                                      // pop rcx as a counter to the push in init
        0xc3,                                      // ret
    });
}

void push::put2NoRecomp (std::vector<uint8_t>& bytes, int64_t const* playfieldData, uint64_t indexFlat) {
    const auto playfieldDataAddress = reinterpret_cast<uint64_t>(playfieldData);
    const auto offsetFlat = indexFlat * 8;

    bytes.insert(bytes.end(), {
        0x48, 0xb8, getByte<0>(playfieldDataAddress), getByte<1>(playfieldDataAddress), getByte<2>(playfieldDataAddress), getByte<3>(playfieldDataAddress), getByte<4>(playfieldDataAddress), getByte<5>(playfieldDataAddress), getByte<6>(playfieldDataAddress), getByte<7>(playfieldDataAddress), // mov rax, playfieldDataAddress
        0x48, 0x8b, 0x54, 0xf7, 0xf8,              // mov rdx, [rdi + rsi * 8 - 8]
        0x48, 0x89, 0x90, getByte<0>(offsetFlat), getByte<1>(offsetFlat), getByte<2>(offsetFlat), getByte<3>(offsetFlat), // mov [rax + offsetFlat], rdx
        0x48, 0xff, 0xce,                          // dec rsi
    });
}

void push::read (std::vector<uint8_t>& bytes, int64_t const* stash, Fun10 read) {
    const auto stashAddress = reinterpret_cast<uint64_t>(stash);
    const auto readAddress = reinterpret_cast<uint64_t>(read);

    bytes.insert(bytes.end(), {
        0x48, 0xb8, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rax, stashAddress
        0x48, 0x89, 0x38,                          // mov [rax], rdi
        0x48, 0x89, 0x70, 0x08,                    // mov [rax + 8], rsi

        0x48, 0xb8, getByte<0>(readAddress), getByte<1>(readAddress), getByte<2>(readAddress), getByte<3>(readAddress), getByte<4>(readAddress), getByte<5>(readAddress), getByte<6>(readAddress), getByte<7>(readAddress), // mov rax, writeAddress
        0xff, 0xd0,                                // call rax

        0x48, 0xb9, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rcx, stashAddress
        0x48, 0x8b, 0x39,                          // mov rdi, [rcx]
        0x48, 0x8b, 0x71, 0x08,                    // mov rsi, [rcx + 8]

        0x48, 0x89, 0x04, 0xf7,                    // mov [rdi + rsi * 8], rax
        0x48, 0xff, 0xc6,                          // inc rsi
    });
}

void push::write (std::vector<uint8_t>& bytes, int64_t const* stash, Fun11 write) {
    const auto stashAddress = reinterpret_cast<uint64_t>(stash);
    const auto writeAddress = reinterpret_cast<uint64_t>(write);

    bytes.insert(bytes.end(), {
        0x48, 0xff, 0xce,                          // dec rsi

        0x48, 0xb8, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rax, stashAddress
        0x48, 0x89, 0x38,                          // mov [rax], rdi
        0x48, 0x89, 0x70, 0x08,                    // mov [rax + 8], rsi

        0x48, 0x8b, 0x3c, 0xf7,                    // mov rdi, [rdi + rsi * 8]

        0x48, 0xb8, getByte<0>(writeAddress), getByte<1>(writeAddress), getByte<2>(writeAddress), getByte<3>(writeAddress), getByte<4>(writeAddress), getByte<5>(writeAddress), getByte<6>(writeAddress), getByte<7>(writeAddress), // mov rax, writeAddress
        0xff, 0xd0,                                // call rax

        0x48, 0xb8, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rax, stashAddress
        0x48, 0x8b, 0x38,                          // mov rdi, [rax]
        0x48, 0x8b, 0x70, 0x08                     // mov rsi, [rax + 8]
    });
}

void push::if_ (std::vector<uint8_t>& bytes, const Path& path, std::vector<PathLink>& pathLinks) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0xff, 0xce,                          // dec rsi
        0x48, 0x85, 0xc0,                          // test rax, rax

        0x0F, 0x84, 0x00, 0x00, 0x00, 0x00,        // jz with 32bit offset set to 00 00 00 00
        0xE9, 0x00, 0x00, 0x00, 0x00               // jmp with 32bit offset set to 00 00 00 00
    });

    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 9, path.next0 });
    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 4, path.next1 });
}

void push::notIf (std::vector<uint8_t>& bytes, bool dup, const Path& path, std::vector<PathLink>& pathLinks) {
    if (dup) {
        bytes.insert(bytes.end(), {
            0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
            0x48, 0x85, 0xc0,                          // test rax, rax

            0x0F, 0x84, 0x00, 0x00, 0x00, 0x00,        // jz with 32bit offset set to 00 00 00 00
            0xE9, 0x00, 0x00, 0x00, 0x00               // jmp with 32bit offset set to 00 00 00 00
        });
    } else {
        bytes.insert(bytes.end(), {
            0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
            0x48, 0xff, 0xce,                          // dec rsi
            0x48, 0x85, 0xc0,                          // test rax, rax

            0x0F, 0x84, 0x00, 0x00, 0x00, 0x00,        // jz with 32bit offset set to 00 00 00 00
            0xE9, 0x00, 0x00, 0x00, 0x00               // jmp with 32bit offset set to 00 00 00 00
        });
    }

    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 9, path.next1 });
    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 4, path.next0 });
}

void compIfTail (std::vector<uint8_t>& bytes, CompType compType, const Path& path, std::vector<PathLink>& pathLinks) {
    const uint8_t compByte = compType == CompType::Gt ? 0x8f
        : compType == CompType::Gte ? 0x8d
        : compType == CompType::Lt ? 0x8c
        : 0x8e;

    bytes.insert(bytes.end(), {
        0x0f, compByte, 0x00, 0x00, 0x00, 0x00,    // jX with 32bit offset set to 00 00 00 00
        0xe9, 0x00, 0x00, 0x00, 0x00               // jmp with 32bit offset set to 00 00 00 00
    });

    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 9, path.next1 });
    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 4, path.next0 });
}

void push::compIf (std::vector<uint8_t>& bytes, CompType compType, const Path& path, std::vector<PathLink>& pathLinks) {
    bytes.insert(bytes.end(), {
        0x48, 0x83, 0xee, 0x02,                    // sub rsi, 2
        0x48, 0x8b, 0x04, 0xf7,                    // mov rax, [rdi + rsi * 8]
        0x48, 0x3b, 0x44, 0xf7, 0x08,              // cmp rax, [rdi + rsi * 8 + 8]
    });

    compIfTail(bytes, compType, path, pathLinks);
}

void push::comp1If (std::vector<uint8_t>& bytes, CompType compType, int64_t value, bool dup, const Path& path, std::vector<PathLink>& pathLinks) {
    if (!dup) {
        bytes.insert(bytes.end(), {
            0x48, 0xff, 0xce                       // dec rsi
        });
    }

    if (std::in_range<std::int8_t>(value)) [[likely]] {
        if (dup) {
            bytes.insert(bytes.end(), {
                0x48, 0x83, 0x7c, 0xf7, 0xf8, getByte<0>(value), // cmp [rdi + rsi * 8 - 8], 8bit
            });
        } else {
            bytes.insert(bytes.end(), {
                0x48, 0x83, 0x3c, 0xf7, getByte<0>(value), // cmp [rdi + rsi * 8], 8bit
            });
        }
    } else if (std::in_range<std::int32_t>(value)) [[likely]] {
        if (dup) {
            bytes.insert(bytes.end(), {
                0x48, 0x81, 0x7c, 0xf7, 0xf8, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), // cmp [rdi + rsi * 8 - 8], 32bit
            });
        } else {
            bytes.insert(bytes.end(), {
                0x48, 0x81, 0x3c, 0xf7, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), // cmp [rdi + rsi * 8], 32bit
            });
        }
    } else {
        if (dup) {
            bytes.insert(bytes.end(), {
                0x48, 0xb8, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), getByte<4>(value), getByte<5>(value), getByte<6>(value), getByte<7>(value), // mov rax, 64bit
                0x48, 0x39, 0x44, 0xf7, 0xf8       // cmp [rdi + rsi * 8 - 8], rax
            });
        } else {
            bytes.insert(bytes.end(), {
                0x48, 0xb8, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), getByte<4>(value), getByte<5>(value), getByte<6>(value), getByte<7>(value), // mov rax, 64bit
                0x48, 0x39, 0x04, 0xf7,            // cmp [rdi + rsi * 8], rax
            });
        }
    }

    compIfTail(bytes, compType, path, pathLinks);
}

void push::rand (std::vector<uint8_t>& bytes, int64_t const* stash, Fun10 rand4, const Path& path, std::vector<PathLink>& pathLinks) {
    const auto stashAddress = reinterpret_cast<uint64_t>(stash);
    const auto rand4Address = reinterpret_cast<uint64_t>(rand4);

    bytes.insert(bytes.end(), {
        0x48, 0xb8, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rax, stashAddress
        0x48, 0x89, 0x38,                          // mov [rax], rdi
        0x48, 0x89, 0x70, 0x08,                    // mov [rax + 8], rsi

        0x48, 0xb8, getByte<0>(rand4Address), getByte<1>(rand4Address), getByte<2>(rand4Address), getByte<3>(rand4Address), getByte<4>(rand4Address), getByte<5>(rand4Address), getByte<6>(rand4Address), getByte<7>(rand4Address), // mov rax, putAddress
        0xff, 0xd0,                                // call rax

        0x48, 0xb9, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rcx, stashAddress
        0x48, 0x8b, 0x39,                          // mov rdi, [rcx]
        0x48, 0x8b, 0x71, 0x08,                    // mov rsi, [rcx + 8]

        0x85, 0xc0,                                // test eax, eax
        0x0F, 0x84, 0x00, 0x00, 0x00, 0x00,        // jz with 32bit offset set to 00 00 00 00
        0x48, 0x83, 0xf8, 0x01,                    // cmp rax, 1
        0x0F, 0x84, 0x00, 0x00, 0x00, 0x00,        // jz with 32bit offset set to 00 00 00 00
        0x48, 0x83, 0xf8, 0x02,                    // cmp rax, 2
        0x0F, 0x84, 0x00, 0x00, 0x00, 0x00,        // jz with 32bit offset set to 00 00 00 00
        0xE9, 0x00, 0x00, 0x00, 0x00,              // jmp with 32bit offset set to 00 00 00 00
    });

    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 29, path.next0 });
    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 19, path.next1 });
    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 9, path.next2 });
    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 4, path.next3 });
}

void push::end (std::vector<uint8_t>& bytes, int64_t const* stash) {
    const auto stashAddress = reinterpret_cast<uint64_t>(stash);

    bytes.insert(bytes.end(), {
        0x48, 0xb9, getByte<0>(stashAddress), getByte<1>(stashAddress), getByte<2>(stashAddress), getByte<3>(stashAddress), getByte<4>(stashAddress), getByte<5>(stashAddress), getByte<6>(stashAddress), getByte<7>(stashAddress), // mov rcx, stashAddress
        0x48, 0x8b, 0x39,                          // mov rdi, [rcx]
        0x48, 0x8b, 0x71, 0x08,                    // mov rsi, [rcx + 8]
        0x59,                                      // pop rcx as a counter to the push in init
        0xc3                                       // ret
    });
}

void push::jump (std::vector<uint8_t>& bytes, Path* path, std::vector<PathLink>& pathLinks) {
    bytes.insert(bytes.end(), {
        0xE9, 0x00, 0x00, 0x00, 0x00               // jmp with 32bit offset set to 00 00 00 00
    });

    pathLinks.push_back({ static_cast<int64_t>(bytes.size()) - 4, path });
}

void push::loopback (std::vector<uint8_t>& bytes, int64_t loopbackIndex) {
    const auto delta = loopbackIndex - bytes.size() - 5;

    // if delta fits in 8bits then use the shorter jump

    bytes.insert(bytes.end(), {
        0xE9, getByte<0>(delta), getByte<1>(delta), getByte<2>(delta), getByte<3>(delta) // jmp with 32bit offset
    });
}

// ---

void push::chain::chainStart (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
    });
}

void push::chain::chainEnd (std::vector<uint8_t>& bytes, int64_t count) {
    const auto offset = -count * 8 - 8;

    if (std::in_range<std::int8_t>(offset)) {
        bytes.insert(bytes.end(), {
            0x48, 0x89, 0x44, 0xf7, getByte<0>(offset), // mov [rdi + rsi * 8 + offset], rax
            0x48, 0x83, 0xee, getByte<0>(count),   // sub rsi, count
        });
    } else {
        bytes.insert(bytes.end(), {
            0x48, 0x89, 0x84, 0xf7, getByte<0>(offset), getByte<1>(offset), getByte<2>(offset), getByte<3>(offset), // mov [rdi + rsi * 8 + offset], rax
            0x48, 0x81, 0xee, getByte<0>(count), getByte<1>(count), getByte<2>(count), getByte<3>(count), // sub rsi, count
        });
    }
}

void push::chain::add (std::vector<uint8_t>& bytes, int64_t count) {
    const auto offset = -count * 8;

    bytes.insert(bytes.end(), {
        0x48, 0x03, 0x44, 0xf7, getByte<0>(offset) // add rax, [rdi + rsi * 8 + offset]
    });
}

void push::chain::add1 (std::vector<uint8_t>& bytes, int64_t value) {
    if (value == -1) {
        bytes.insert(bytes.end(), {
            0x48, 0xff, 0xc8                       // dec rax
        });
        return;
    }

    if (value == 0) {
        // nothing
        return;
    }

    if (value == 1) {
        bytes.insert(bytes.end(), {
            0x48, 0xff, 0xc0                       // inc rax
        });
        return;
    }

    if (std::in_range<std::int8_t>(value)) {
        bytes.insert(bytes.end(), {
            0x48, 0x83, 0xc0, getByte<0>(value)    // add rax, 8bit
        });
        return;
    }

    if (std::in_range<std::int32_t>(value)) {
        bytes.insert(bytes.end(), {
            0x48, 0x05, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value) // add rax, 32bit
        });
        return;
    }

    bytes.insert(bytes.end(), {
        0x48, 0xb9, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), getByte<4>(value), getByte<5>(value), getByte<6>(value), getByte<7>(value), // mov rcx, 64bit
        0x48, 0x01, 0xc8,                          // add rax, rcx
    });
}

void push::chain::sub (std::vector<uint8_t>& bytes, int64_t count) {
    const auto offset = -count * 8;

    bytes.insert(bytes.end(), {
        0x48, 0xf7, 0xd8,                          // neg rax
        0x48, 0x03, 0x44, 0xf7, getByte<0>(offset) // add rax, [rdi + rsi * 8 + offset]
    });
}

void push::chain::subRev (std::vector<uint8_t>& bytes, int64_t count) {
    const auto offset = -count * 8;

    bytes.insert(bytes.end(), {
        0x48, 0x2b, 0x44, 0xf7, getByte<0>(offset) // sub rax, [rdi + rsi * 8 + offset]
    });
}

void push::chain::mul (std::vector<uint8_t>& bytes, int64_t count) {
    const auto offset = -count * 8;

    bytes.insert(bytes.end(), {
        0x48, 0x0f, 0xaf, 0x44, 0xf7, getByte<0>(offset) // imul rax, [rdi + rsi * 8 + offset]
    });
}

void push::chain::mul1 (std::vector<uint8_t>& bytes, int64_t value) {
    if (value == -1) {
        bytes.insert(bytes.end(), {
            0x48, 0xf7, 0xd8,                      // neg rax
        });
        return;
    }

    if (value == 0) {
        bytes.insert(bytes.end(), {
            0x31, 0xc0,                            // xor eax, eax
        });
        return;
    }

    if (value == 1) {
        // nothing
        return;
    }

    if (value == 2) {
        bytes.insert(bytes.end(), {
            0x48, 0xd1, 0xe0,                      // shl rax
        });
        return;
    }

    if (value == 3) {
        bytes.insert(bytes.end(), {
            0x48, 0x8d, 0x04, 0x40                 // lea rax, [rax + rax * 2]
        });
        return;
    }

    if (value == 5) {
        bytes.insert(bytes.end(), {
            0x48, 0x8d, 0x04, 0x80                 // lea rax, [rax + rax * 4]
        });
        return;
    }

    if ((value & (value - 1)) == 0) {
        // powers of two
        const auto shift = std::bit_width(static_cast<uint64_t>(value)) - 1;
        bytes.insert(bytes.end(), {
            0x48, 0xc1, 0xe0, getByte<0>(shift)    // shl rax, shift
        });
        return;
    }

    if (std::in_range<std::int8_t>(value)) [[likely]] {
        bytes.insert(bytes.end(), {
            0x48, 0x6b, 0xc0, getByte<0>(value)    // imul rax, 8bit
        });
        return;
    }

    if (std::in_range<std::int32_t>(value)) {
        bytes.insert(bytes.end(), {
            0x48, 0x69, 0xc0, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), // imul rax, 32bit
        });
        return;
    }

    bytes.insert(bytes.end(), {
        0x48, 0xb9, getByte<0>(value), getByte<1>(value), getByte<2>(value), getByte<3>(value), getByte<4>(value), getByte<5>(value), getByte<6>(value), getByte<7>(value), // mov rcx, 64bit
        0x48, 0x0f, 0xaf, 0xc1                     // imul rax, rcx
    });
}

void push::chain::sqr (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x0f, 0xaf, 0xc0                     // imul rax, rax
    });
}

void writeOffset32 (std::vector<uint8_t>& bytes, uint64_t index, int32_t offset) {
    bytes[index + 0] = getByte<0>(offset);
    bytes[index + 1] = getByte<1>(offset);
    bytes[index + 2] = getByte<2>(offset);
    bytes[index + 3] = getByte<3>(offset);
}