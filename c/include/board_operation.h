#ifndef BOARD_OPERATION_H
#define BOARD_OPERATION_H

#include <vector>
#include <Board.h>

unsigned long long integerPow(unsigned long long base, unsigned int exponent);

/**
 * @brief Count the number of tokens in the column that is still on the board.
 * @param col Column to count.
 * @return Number of movable tokens.
 */
size_t countMovableTokens(const std::vector<std::pair<int, int>>& col);

// Each move is represented as a list of tokens indices to push (std::vector<int>).
using PusherMove = std::vector<int>;

enum class Player {
    PUSHER, REMOVER, NONE
};

/**
 * @brief Checks the winner of the current board. Does not do predictions.
 * @param board Target board to check.
 * @return Winner.
 */
Player checkWinner(Board& board);

/**
 * @brief A faster version of Board::make_pusher_board.
 * @param board Board to move.
 * @param tokens Tokens to push forward.
 */
void fast_make_pusher_board(Board& board, const PusherMove& tokensToMove);

/**
 * @brief A faster version of Board::make_remover_board.
 * @param board Board to move.
 * @param col Column number chosen by remover.
 */
void faster_make_remover_board(Board& board, int col);

/**
 * @brief Generates a powerset of a given set.
 * @tparam T Type of element.
 * @param vec Original set.
 * @param powerset Resulting powerset is stored here.
 */
template<typename T>
void generatePowerset(const std::vector<T>& vec, std::vector<std::vector<T>>& powerset) {
    powerset.clear();

    // There are 2^n possible subsets for a set of size n
    size_t powSetCount = integerPow(2, (unsigned int)vec.size());

    for (size_t i = 0; i < powSetCount; i++) {
        std::vector<T>& subset = powerset.emplace_back();
        for (int j = 0; j < vec.size(); j++) {
            // Check if jth element is in the current subset (counter)
            if (i & (1ULL << j)) {
                subset.push_back(vec[j]);
            }
        }
    }
}

/**
 * @brief Finds all possible pusher moves. Not pruned. May have duplicates.
 * @param board The current board.
 * @return All pusher moves.
 */
std::vector<PusherMove> getAllPusherMoves(const Board& board);

/**
 * @brief Finds all possible pusher moves. Prunes unpromising moves.
 * @param board The current board.
 * @return All pusher moves.
 */
std::vector<PusherMove> getAllPusherMovesPruned(const Board& board);

#endif //BOARD_OPERATION_H
