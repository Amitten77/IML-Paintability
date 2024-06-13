#ifndef INIT_H
#define INIT_H

#include <string>
#include "json.hpp"

/**
 * @brief Load the pairs of k*n from the given JSON 2D array.
 * @param target The target to load into.
 * @param arr The JSON array.
 */
void load_k_and_n(std::vector<std::pair<int, int>>& target, const nlohmann::json& arr);

/**
 * @brief Generate the file names associated with the given n, k, and goal.
 * @param n Number of columns.
 * @param k Tokens in each column.
 * @param goal Target row to reach.
 * @return Two strings, the path to the winning board and the losing board.
 */
std::pair<std::string, std::string> getFileNames(int n, int k, int goal);

/// @brief Initialize the board with the given pairs of k and n.
void createBoard(
        std::vector<std::vector<std::pair<int, int>>>& board, int& N, int& K,
        const std::vector<std::pair<int, int>>& pairsOfKAndN);

/**
 * @brief Given a Board with N columns and K tokens, initializes subset_graph and num_graph
 * mapping each subset of tokens to a move number
 * @param N Numbers of Columns
 * @param K Tokens in each Column
 */
void initMap(int N, int K);

#endif // INIT_H
