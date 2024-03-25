#include <ranges>
#include "hash.h"

void decodeMove(EncodedMove encoded, PusherMove& decoded, int k, size_t col) {
    for (int i = 0; i < k; i++) {
        if (encoded % 2) decoded.push_back((int)col * k + i);
        encoded /= 2;
    }
}

EncodedColState encodeColState(const std::vector<int>& column, unsigned int goal) {
    EncodedColState encoded = 0;
    for (int row : column  | std::views::reverse) {
        encoded += (row + 1);
        encoded *= (goal + 2);
    }
    return encoded;
}

std::string toString(EncodedMove move, int k) {
    std::string result;
    for (size_t _ = 0; _ < k; _++) {
        result += std::to_string(move % 2);
        move /= 2;
    }
    return result;
}
