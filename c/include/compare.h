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
 * @param board The target board.
 * @param boards A list of winning boards.
 * @return Whether it is winning.
 */

bool bfs(const std::vector<std::unordered_set<int>>& relation, std::vector<int>& distance, std::vector<int>& pairU, std::vector<int>& pairV, int n);

bool dfs(int u, const std::vector<std::unordered_set<int>>& relation, std::vector<int>& distance, std::vector<int>& pairU, std::vector<int>& pairV);

bool checkHopcroft(const std::vector<std::unordered_set<int>>& relation, int n);

#endif // COMPARE_H
