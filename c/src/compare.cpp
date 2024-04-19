#include <algorithm>
#include <functional>
#include <ranges>
#include "../include/compare.h"


//#define USE_HOPCROFT_KARPP
#define HALLS


int checkHallsCondition(const std::vector<std::unordered_set<int>>& relation, int n) {
    std::vector<int> subset;
    std::function<bool(int)> backtrack = [&](int start) -> bool {
        std::unordered_set<int> neighbors;
        for (int value : subset) {
            neighbors.insert(relation[value].begin(), relation[value].end());
        }
        if (neighbors.size() < subset.size()) {
            return false;
        }
        if (start == n) {
            return true;
        }

        for (int i = start; i < n; ++i) {
            subset.push_back(i);
            if (!backtrack(i + 1)) {
                return false;
            }
            subset.pop_back();
        }

        return true;
    };

    return backtrack(0);
}


CompResult compareBoards(const Board& board1, const Board& board2, Purpose purpose) {
    if (board1.n != board2.n || board1.k != board2.k) {
        return CompResult::INCOMPARABLE;
    }
    int n = board1.n;
    int k = board1.k;
    bool possLess = true;
    bool possMore = true;
    if (purpose == Purpose::LESS) {
        possMore = false;
    }
    if (purpose == Purpose::GREATER) {
        possLess = false;
    }
    if (board1.num_tokens > board2.num_tokens) {
        possLess = false;
    }

    if (board1.num_tokens < board2.num_tokens) {
        possMore = false;
    }
    if (!possLess && !possMore) {
        return CompResult::INCOMPARABLE;
    }
    std::vector<int> board1bst;
    std::vector<int> board2bst;
    for (int i = 0; i < n; ++i) {
        board1bst.push_back(board1.board[board1.get_index(i, k-1)]);
        board2bst.push_back(board2.board[board2.get_index(i, k-1)]);
    }
    std::sort(board1bst.begin(), board1bst.end());
    std::sort(board2bst.begin(), board2bst.end());
    for (int i = 0; i < n; ++i) {
        if (board1bst[i] > board2bst[i]) {
            possLess = false;
        }
        if (board1bst[i] < board2bst[i]) {
            possMore = false;
        }
        if (!possLess && !possMore) {
            return CompResult::INCOMPARABLE;
        }
    }

    std::vector<std::unordered_set<int>> lessThanRelation(n), greaterThanRelation(n);



    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            bool less = true, greater = true;
            for (int item = 0; item < board1.k; ++item) {
                int item1 = board1.board[board1.get_index(i, item)];
                int item2 = board2.board[board2.get_index(j, item)];
                if (item1 > item2) less = false;
                if (item1 < item2) greater = false;
                if (!greater && !less) break;
            }
            if (less) lessThanRelation[i].insert(j);
            if (greater) greaterThanRelation[i].insert(j);
        }
        if (lessThanRelation[i].size() == 0) {
            possLess = false;
        }
        if (greaterThanRelation[i].size() == 0) {
            possMore = false;
        }
        if (!possLess && !possMore) {
            return CompResult::INCOMPARABLE;
        }
    }
    if (!possLess && !possMore) {
        return CompResult::INCOMPARABLE;
    }
    // Check Hall's condition for both relations
    bool isMore = false;
    bool isLess = false;
    if (possMore && !possLess) {
        isMore = checkHallsCondition(greaterThanRelation, n);
    } else if (possLess && !possMore) {
        isLess = checkHallsCondition(lessThanRelation, n);
    } else {
        isMore = checkHallsCondition(greaterThanRelation, n);
        isLess = checkHallsCondition(lessThanRelation, n);
    }
    if (isMore && isLess) return CompResult::EQUAL;
    if (isLess) return CompResult::LESS;
    if (isMore) return CompResult::GREATER;
    return CompResult::INCOMPARABLE;
}

bool boardIsWinning(const Board& board, const std::vector<Board>& winningBoards) {
    return std::ranges::any_of(winningBoards, [&board](const Board& other) {
        return compareBoards(board, other, Purpose::GREATER) == CompResult::GREATER;
    });
}
