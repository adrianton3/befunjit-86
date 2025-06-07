#include <cstdint>
#include <sys/mman.h>
#include <vector>
#include <cstring>

#include "alloc.h"


uint8_t* alloc (const std::vector<uint8_t>& codeBytes) {
    void* raw = mmap(
        nullptr,
        codeBytes.size(), // * 1
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_PRIVATE|MAP_ANONYMOUS,
        -1,
        0
    );

    if (raw == MAP_FAILED) {
        return nullptr;
    }

    auto address = static_cast<uint8_t*>(raw);

    memcpy(address, codeBytes.data(), codeBytes.size());

    const auto protectResult = mprotect(raw, codeBytes.size(), PROT_READ|PROT_EXEC);

    if (protectResult == -1) {
        munmap(raw, codeBytes.size());
        return nullptr;
    }

    return address;
}

int64_t call (uint8_t* codeFunAddress, int64_t* stack, int64_t offset) {
    const auto codeFun = reinterpret_cast<CodeFun>(codeFunAddress);
    return codeFun(stack, offset);
}

void dealloc (uint8_t* codeFunAddress, size_t size) {
    munmap(codeFunAddress, size);
}