#ifndef COMPARE_H
#define COMPARE_H

#include <vector>
#include <unordered_set>
#include "Board.h"

/**
 * @class Purpose
 * @brief
 */
enum class Purpose { LESS, GREATER, BOTH };
enum class CompResult { LESS, GREATER, EQUAL, INCOMPARABLE };

inline std::string toString(Purpose purpose) {
    switch (purpose) {
        case Purpose::LESS:
            return "LESS";
        case Purpose::GREATER:
            return "GREATER";
        case Purpose::BOTH:
            return "BOTH";
    }
    return "";
}

inline std::string toString(CompResult result) {
    switch (result) {
        case CompResult::LESS:
            return "LESS";
        case CompResult::GREATER:
            return "GREATER";
        case CompResult::EQUAL:
            return "EQUAL";
        case CompResult::INCOMPARABLE:
            return "INCOMPARABLE";
    }
    return "";
}

int checkHallsCondition(const std::vector<std::unordered_set<int>>& relation, int n);

/**
 * @param col1 First column, using the standard math notation, sorted from largest to smallest.
 * @param col2 Second column, using the standard math notation, sorted from largest to smallest.
 * @return Compare result.
 */
CompResult compareSortedCols(const std::vector<int>& col1, const std::vector<int>& col2);

/// @brief Same function, but different param types.
CompResult compareBoardCols(const std::vector<std::pair<int, int>>& col1, const std::vector<std::pair<int, int>>& col2);

/**
 * @brief Comparing two game states.
 * @param board1 First game state.
 * @param board2 Second game state.
 * @param purpose Can be "GREATER", "LESS", or "BOTH".
 *  If is "LESS": Only care about CompResult::LESS or otherwise.
 *  If is "GREATER": Only care about CompResult::GREATER or otherwise.
 * @return Compare result.
 */
CompResult compareBoards(const Board& board1, const Board& board2, Purpose purpose = Purpose::BOTH);

/**
 * @brief Compare a board with winning boards to check if it is winning.
 * @tparam T Iterator type.
 * @param board The target board.
 * @param winningBoardsBegin Starting iterator of a list of winning boards.
 * @param winningBoardsEnd Ending iterator of a list of winning boards.
 * @return Whether it is winning.
 */
template <typename T>
bool boardIsWinning(const Board& board, T winningBoardsBegin, T winningBoardsEnd) {
    return std::ranges::any_of(winningBoardsBegin, winningBoardsEnd, [&board](const Board& other) {
        return compareBoards(board, other, Purpose::GREATER) == CompResult::GREATER;
    });
}

/**
 * @brief Compare a board with losing boards to check if it is losing.
 * @tparam T Iterator type.
 * @param board The target board.
 * @param losingBoardsBegin Starting iterator of a list of losing boards.
 * @param losingBoardsEnd Ending iterator of a list of losing boards.
 * @return Whether it is losing.
 */
template <typename T>
bool boardIsLosing(const Board& board, T losingBoardsBegin, T losingBoardsEnd) {
    return std::ranges::any_of(losingBoardsBegin, losingBoardsEnd, [&board](const Board& other) {
        return compareBoards(board, other, Purpose::LESS) == CompResult::LESS;
    });
}

#endif // COMPARE_H
