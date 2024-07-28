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

std::vector<std::vector<int>> product(const std::vector<std::vector<std::vector<int>>>& lists) {
    std::vector<std::vector<int>> result;

    for (const auto& list : lists) {
        if (list.empty()) return {{}};
    }
    result.emplace_back();

    for (const auto& list : lists) {
        std::vector<std::vector<int>> tempResult;

        for (const auto& existingCombo : result) {
            for (const auto& item : list) {
                std::vector<int> newCombo = existingCombo;
                newCombo.insert(newCombo.end(), item.begin(), item.end());
                tempResult.push_back(newCombo);
            }
        }

        result = std::move(tempResult);
    }

    return result;
}
