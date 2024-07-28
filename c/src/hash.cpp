#include <ranges>
#include "hash.h"

void decodeMove(EncodedMove encoded, PusherMove& decoded, int k, size_t col) {
    for (int i = 0; i < k; i++) {
        if (encoded % 2) decoded.push_back((int)col * k + (k - i - 1));
        encoded /= 2;
    }
}

EncodedColumnState encodeColState(const ColumnState& column, unsigned int goal) {
    EncodedColumnState encoded = 0;
    for (int row : column | std::views::reverse) {
        encoded += (row + 1);
        encoded *= (goal + 2);
    }
    return encoded;
}

void applyMoveToColumn(ColumnState& column, EncodedMove move) {
    for (int& r : column) {
        if (move == 0) break;
        bool shouldMove = move % 2;
        move /= 2;
        if (shouldMove && r != -1) {
            r++;
        }
    }
    std::sort(column.begin(), column.end(), std::greater<>());
}

std::string toString(EncodedMove move, size_t k) {
    std::string result;
    for (size_t _ = 0; _ < k; _++) {
        result.insert(0, std::to_string(move % 2));
        move /= 2;
    }
    return result;
}
