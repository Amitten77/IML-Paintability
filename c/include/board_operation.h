#ifndef BOARD_OPERATION_H
#define BOARD_OPERATION_H

#include <functional>
#include <vector>
#include "board.h"

/// @brief Integer version of std::pow.
size_t integerPow(size_t base, size_t exponent);

/**
 * @brief Count the number of movable tokens (i.e. tokens not removed) in a single column.
 * @param col The target column.
 * @return Number of movable tokens.
 */
size_t countMovableTokens(const std::vector<std::pair<int, int>>& col);

bool operator==(const Board& board1, const Board& board2);

/**
 * @brief Checks the winner of the current board.
 * @param board Target board to check.
 * @return Winner if game is over, Player::NONE otherwise.
 *
 * Does not do predictions.
 */
Player checkWinner(Board& board);

/**
 * @brief Applies a Pusher move to the board in-place.
 * @param board Board to move.
 * @param tokens Indices of tokens to push forward.
 *
 * A faster version of Board::make_pusher_board.
 * Does not check for duplicated indices. Moves the token regardless of whether it is already moved.
 */
void applyPusherMove(Board& board, const PusherMove& tokensToMove);

/**
 * @brief Applies a Remover move to the board in-place.
 * @param board Board to move.
 * @param col Column number chosen by remover.
 *
 * A faster version of Board::make_remover_board.
 */
void applyRemoverMove(Board& board, int col);

/**
 * @brief Finds all possible pusher moves. Not pruned. May have duplicates.
 * @param board The current board.
 * @return All pusher moves.
 */
std::vector<PusherMove> getAllPusherMoves(const Board& board);

/**
 * @brief Finds all possible pusher moves. Prunes unpromising moves.
 * @param board The current board.
 * @param moves Where to output the pusher moves to.
 * @param verbose Defines how much calculation details to log.
 */
void getAllPusherMovesPruned(const Board& board, std::vector<PusherMove>& moves, int verbose = 0);

/**
 * @brief Finds all possible remover moves. Prunes unpromising moves.
 * @param board The current board.
 * @param moves Where to output the remover moves to.
 * @param verbose Defines how much calculation details to log.
 */
void getAllRemoverMovesPruned(const Board& board, std::vector<int>& moves, int verbose = 0);

/**
 * @brief Starting from a game state, the Pusher makes one move.
 * @param board The current board. The Pusher moves next.
 * @param output Where to output all possible resulting states. WILL APPEND TO BACK.
 * @param pred A predicate function that filters whether or not to include a resulting state (return true to include).
 */
void stepPusher(const Board& board, std::vector<Board>& output, const std::function<bool(const Board&)>& pred);

/**
 * @brief Starting from a game state, the Remover makes one move.
 * @param board The current board. The Remover moves next.
 * @param output Where to output all possible resulting states. WILL APPEND TO BACK.
 * @param pred A predicate function that filters whether or not to include a resulting state (return true to include).
 */
void stepRemover(const Board& board, std::vector<Board>& output, const std::function<bool(const Board&)>& pred);

/**
 * @brief Starting from a game state, the Pusher and the Remover each makes one move.
 * @param board The current board. The Pusher moves next.
 * @param output Where to output all possible resulting states. WILL APPEND TO BACK.
 * @param pred A predicate function that filters whether or not to include a resulting state (return true to include).
 */
void step(const Board& board, std::vector<Board>& output, const std::function<bool(const Board&)>& pred);

#endif //BOARD_OPERATION_H
