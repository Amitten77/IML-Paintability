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

int checkHallsCondition(const std::vector<std::unordered_set<int>>& relation, int n);

/**
 * @param col1 First column, using the standard math notation, sorted from largest to smallest.
 * @param col2 Second column, using the standard math notation, sorted from largest to smallest.
 * @return Compare result.
 */
CompResult compareSortedCols(const std::vector<int>& col1, const std::vector<int>& col2);

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

#endif // COMPARE_H
