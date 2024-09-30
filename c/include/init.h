/**
 * @file init.h
 * @brief Initialize the game board.
 *
 * This file contains functions to initialize the game board according to the configuration file. See README.md for more
 * information on the configuration file.
 */

#ifndef INIT_H
#define INIT_H

#include "json.hpp"
#include "game_state.h"

/**
 * @brief Create the initial game state using the specified configurations.
 * @param config The configurations.
 * @return The initial game state.
 */
GameState initGameState(const nlohmann::json& config);

#endif // INIT_H
