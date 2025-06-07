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

void push::sub (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf8,              // mov rax, [rdi + rsi * 8 - 8]
        0x48, 0x29, 0x44, 0xf7, 0xf0,              // sub [rdi + rsi * 8 - 16], rax
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

void push::div (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x44, 0xf7, 0xf0,              // mov rax, [rdi + rsi * 8 - 16]
        0x48, 0x99,                                // cqo
        0x48, 0xf7, 0x7c, 0xf7, 0xf8,              // idiv qword [rdi + rsi * 8 - 8]
        0x48, 0x89, 0x44, 0xf7, 0xf0,              // mov [rdi + rsi * 8 - 16], rax
        0x48, 0xff, 0xce                           // dec rsi
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

void push::not_ (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x4c, 0xf7, 0xf8,              // mov rcx, [rdi + rsi * 8 - 8]
        0x31, 0xc0,                                // xor eax, eax
        0x48, 0x85, 0xc9,                          // test rcx, rcx
        0x0f, 0x94, 0xc0,                          // sete al
        0x48, 0x89, 0x44, 0xf7, 0xf8               // mov qword [rdi + rsi * 8 - 8], rax
    });
}

 void push::gt (std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), {
        0x48, 0x8b, 0x4c, 0xf7, 0xf8,              // mov rcx, [rdi + rsi * 8 - 8]
        0x48, 0x8b, 0x54, 0xf7, 0xf0,              // mov rdx, [rdi + rsi * 8 - 16]
        0x48, 0xff, 0xce,                          // dec rsi
        0x31, 0xc0,                                // xor eax, eax
        0x48, 0x39, 0xca,                          // cmp rdx, rcx
        0x0f, 0x9f, 0xc0,                          // setg al
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
        0xE9, getByte<0>(delta), getByte<1>(delta), getByte<2>(delta), getByte<3>(delta)               // jmp with 32bit offset
    });
}