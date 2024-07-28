/**
 * @file init.h
 * @brief Initialize the game board.
 *
 * This file contains functions to initialize the game board according to the configuration file. A configuration file
 * has the following format:
 *
 * ```json
 * {
 *   todo
 * }
 * ```
 *
 * todo
 */

#ifndef INIT_H
#define INIT_H

#include <filesystem>
#include <string>
#include "json.hpp"

/**
 * @brief Load the pairs of k*n from the given JSON 2D array.
 * @param arr The JSON array.
 * @return A vector of pairs of k_ and n_.
 *
 * Each pair of k_ and n_ represents n_ columns with k_ chips on row 0. The actual n is the sum of all n_, and the
 * actual k is the maximum of all k_.
 *
 * E.g.
 * ```json
 * [[2, 3], [3, 4]]
 * ```
 * refers to the graph `K_{2*3, 3*4}`, which contains 7 columns, 3 of which have 2 chips and 4 of which have 3 chips.
 * It has n = 7 and k = 3.
 */
std::vector<std::pair<size_t, size_t>> loadKAndN(const nlohmann::json& arr);

/**
 * @brief Generate the file names associated with the given n, k, and goal.
 * @param n Number of columns.
 * @param k Tokens in each column.
 * @param goal Target row to reach.
 * @return The paths to the winning board and the losing board.
 */
std::pair<std::filesystem::path, std::filesystem::path> getFileNames(size_t n, size_t k, int goal);

/**
 * @brief Initialize the board with the given pairs of k and n.
 */
Board createBoard(const std::vector<std::pair<size_t, size_t>>& pairsOfKAndN);

#endif // INIT_H
