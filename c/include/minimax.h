/**
 * @file minimax.h
 * @brief Implements the minimax algorithm.
 *
 * This file contains the main algorithm for finding the winner of a given board, assuming that the Pusher and Remover
 * play optimally. The algorithm is based on the minimax algorithm.
 */

#ifndef MINIMAX_H
#define MINIMAX_H

#include "archive.h"
#include "game_state.h"

/**
 * @brief Runs minimax to search all possible moves of both players.
 * @param initialState The initial game state.
 * @param archive The archive to store the winning and losing states.
 * @param hoursPerSave The interval to save the temporary archive.
 * @param threads The number of threads.
 * @param count The number of states visited.
 * @return Predicted winner.
 */
Player minimax(const GameState& initialState, Archive& archive, double hoursPerSave, size_t threads, size_t& count);

#endif // MINIMAX_H
