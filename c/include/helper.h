/**
 * @file helper.h
 * @brief This file contains helper functions for the project.
 */

#ifndef HELPER_H // Include guard
#define HELPER_H

#include <filesystem>
#include <vector>

/// @brief Integer version of std::pow.
size_t integerPow(size_t base, size_t exponent);

/**
 * @brief Count the number of movable chips (i.e. chips not removed) in a single column.
 * @param column The target column.
 * @return Number of movable tokens.
 */
size_t countMovableChips(const std::vector<int>& column);

/**
 * @brief Generate the file name associated with the given n, k, and goal.
 * @param n Number of columns.
 * @param k Tokens in each column.
 * @param goal Target row to reach.
 * @param suffix Optional suffix to append to the file name (before the extension `.txt`).
 * @return The paths to the winning board and the losing board.
 */
std::filesystem::path getFilename(size_t n, size_t k, int goal, const std::string& suffix = "");

/// @return The current time in the format of "YYYY-mm-dd_HH-MM".
std::string getCurrentTime();

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
    size_t powSetCount = integerPow(2, vec.size());

    for (size_t i = 0; i < powSetCount; i++) {
        std::vector<T>& subset = powerset.emplace_back();
        for (size_t j = 0; j < vec.size(); j++) {
            // Check if jth element is in the current subset (counter)
            if (i & (1ULL << j)) {
                subset.push_back(vec[j]);
            }
        }
    }

    return powerset;
}

#endif // HELPER_H
