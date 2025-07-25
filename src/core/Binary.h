#ifndef BINARY_H
#define BINARY_H

#include <cstdint>
#include <vector>


struct Binary {
    Binary () : size { 0 }, capacity { 0 }, code { nullptr } {}

    Binary (const Binary&) = delete;

    Binary& operator= (const Binary&) = delete;

    void write (const std::vector<uint8_t>&);

    void call (int64_t*, int64_t) const;

    ~Binary();

private:
    void alloc (const std::vector<uint8_t>&);

    void refresh (const std::vector<uint8_t>&);

    void dealloc ();

    uint32_t size;
    uint32_t capacity;
    uint8_t* code;
};


#endif //BINARY_H
