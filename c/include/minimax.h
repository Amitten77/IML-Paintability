/**
 * @file minimax.h
 * @brief Minimax algorithm with alpha-beta pruning
 *
 * This file contains the main algorithm for finding the score of a given board, assuming that the Pusher and Remover
 * play optimally. The algorithm is based on the minimax algorithm with alpha-beta pruning.
 */

#ifndef MINIMAX_H
#define MINIMAX_H

#include "archive.h"
#include "game_state.h"

/**
 * @brief Runs minimax to search all possible moves of both players.
 * @param initialState The initial game state.
 * @param archive The archive to store the winning and losing states.
 * @param count The number of states visited.
 * @return Predicted winner.
 */
Player minimax(const GameState& initialState, const Archive& archive, size_t& count);

#endif // MINIMAX_H
