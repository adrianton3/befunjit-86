#include <string>

#include "CompType.h"


std::string stringify (CompType type) {
    return type == CompType::Gt ? std::string { "Gt" }
        : type == CompType::Gte ? std::string { "Gte" }
        : type == CompType::Lt ? std::string { "Lt" }
        : std::string { "Lte" };
}
