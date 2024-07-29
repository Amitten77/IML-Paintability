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
#include "game_state.h"

/**
 * @brief Generate the file names associated with the given n, k, and goal.
 * @param n Number of columns.
 * @param k Tokens in each column.
 * @param goal Target row to reach.
 * @return The paths to the winning board and the losing board.
 */
std::pair<std::filesystem::path, std::filesystem::path> getFileNames(size_t n, size_t k, int goal);

/**
 * @brief Create the initial game state using the specified configurations.
 * @param config The configurations.
 * @return The initial game state.
 */
GameState initGameState(const nlohmann::json& config);

#endif // INIT_H
