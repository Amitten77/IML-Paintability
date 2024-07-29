#include "helper.h"

size_t integerPow(size_t base, size_t exponent) {
    size_t result = 1;
    for (size_t i = 0; i < exponent; i++) result *= base;
    return result;
}

size_t countMovableChips(const std::vector<int>& column) {
    size_t count = 0;
    for (int r : column) {
        if (r < 0) break;
        count++;
    }
    return count;
}
