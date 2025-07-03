#include <cstdint>
#include <cstring>
#include <sys/mman.h>
#include <vector>

#include "Binary.h"


void Binary::write (const std::vector<uint8_t>& bytes) {
    if (code == nullptr) {
        alloc(bytes);
        return;
    }

    if (bytes.size() <= capacity) {
        refresh(bytes);
        return;
    }

    dealloc();
    alloc(bytes);
}

void Binary::call (int64_t* stack, int64_t offset) {
    typedef void (*CodeFun)(int64_t* stack, int64_t offset);

    const auto codeFun = reinterpret_cast<CodeFun>(code);
    codeFun(stack, offset);
}

Binary::~Binary () {
    if (code != nullptr) {
        munmap(code, capacity);
    }
}

void Binary::alloc (const std::vector<uint8_t>& bytes) {
    size = bytes.size();
    capacity = size + size / 4; // overallocate a bit

    void* raw = mmap(
        nullptr,
        capacity, // * 1
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_PRIVATE|MAP_ANONYMOUS,
        -1,
        0
    );

    if (raw == MAP_FAILED) {
        size = 0;
        capacity = 0;
        code = nullptr;
        return;
    }

    code = static_cast<uint8_t*>(raw);

    memcpy(code, bytes.data(), bytes.size());

    const auto protectResult = mprotect(raw, capacity, PROT_READ|PROT_EXEC);

    if (protectResult != 0) {
        munmap(raw, capacity);
        size = 0;
        capacity = 0;
        code = nullptr;
    }
}

void Binary::refresh (const std::vector<uint8_t>& bytes) {
    void* raw = code;

    const auto deprotectResult = mprotect(raw, capacity, PROT_WRITE);

    if (deprotectResult != 0) {
        munmap(raw, capacity);
        size = 0;
        capacity = 0;
        code = nullptr;
        return;
    }

    size = bytes.size();
    memcpy(code, bytes.data(), size);

    const auto protectResult = mprotect(raw, capacity, PROT_READ|PROT_EXEC);

    if (protectResult != 0) {
        munmap(raw, capacity);
        size = 0;
        capacity = 0;
        code = nullptr;
    }
}

void Binary::dealloc () {
    munmap(code, capacity);

    size = 0;
    capacity = 0;
    code = nullptr;
}