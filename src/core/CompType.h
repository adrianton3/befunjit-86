#ifndef COMPTYPE_H
#define COMPTYPE_H

#include <cstdint>
#include <string>


enum class CompType : uint8_t {
    Gt,
    Gte,
    Lt,
    Lte
};

std::string stringify (CompType compType);


#endif //COMPTYPE_H
