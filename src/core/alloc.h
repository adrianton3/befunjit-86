#ifndef ALLOC_H
#define ALLOC_H

#include <cstdint>
#include <vector>


uint8_t* alloc (const std::vector<uint8_t>& codeBytes);

typedef int64_t (*CodeFun)(int64_t* stack, int64_t offset);

int64_t call (uint8_t* codeFunAddress, int64_t* stack, int64_t offset);

void dealloc (uint8_t* codeFunAddress, size_t size);


#endif //ALLOC_H
