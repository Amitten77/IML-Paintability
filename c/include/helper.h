#ifndef HELPER_H // Include guard
#define HELPER_H

#include <vector>
#include <string>

class Board;

/// @brief Integer version of std::pow.
size_t integerPow(size_t base, size_t exponent);

/**
 * @brief Count the number of movable chips (i.e. chips not removed) in a single column.
 * @param column The target column.
 * @return Number of movable tokens.
 */
size_t countMovableChips(const std::vector<int>& column);

/**
 * @brief Generates a powerset of a given set.
 * @tparam T Type of element.
 * @param vec Original set.
 * @param powerset Resulting powerset is stored here.
 */
template<typename T>
std::vector<std::vector<T>> powerset(const std::vector<T>& vec) {
    std::vector<std::vector<T>> powerset;

    // There are 2^n possible subsets for a set of size n
    size_t powSetCount = integerPow(2, (unsigned int)vec.size());

    for (size_t i = 0; i < powSetCount; i++) {
        std::vector<T>& subset = powerset.emplace_back();
        for (int j = 0; j < (int)vec.size(); j++) {
            // Check if jth element is in the current subset (counter)
            if (i & (1ULL << j)) {
                subset.push_back(vec[j]);
            }
        }
    }

    return powerset;
}

/**
 * @brief Takes the cartestion product of multiple vectors
*/
std::vector<std::vector<int>> product(const std::vector<std::vector<std::vector<int>>>& lists);

#endif // HELPER_H
