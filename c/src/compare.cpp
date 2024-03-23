#include <algorithm>
#include <functional>
#include "../include/compare.h"
#include "../include/graph.h"

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

CompResult compareSortedCols(const std::vector<int>& col1, const std::vector<int>& col2) {
    bool col1IsLess = true, col2IsLess = true;

    for (size_t i = 0; i < col1.size(); i++) {
        // Compare i-th element
        if (col1[i] > col2[i]) col1IsLess = false;
        if (col2[i] > col1[i]) col2IsLess = false;
    }

    if (col1IsLess && col2IsLess) return CompResult::EQUAL;
    else if (col1IsLess) return CompResult::LESS;
    else if (col2IsLess) return CompResult::GREATER;
    else return CompResult::INCOMPARABLE;
}

#ifdef USE_HOPCROFT_KARP

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
        board1bst.push_back(board1.board[i][k - 1].first);
        board2bst.push_back(board2.board[i][k - 1].first);
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

    bool firstIsLess = false, secondIsLess = false;
    // Represent the boards in the standard notation
    std::vector<std::vector<int>> gameState1, gameState2;
    gameState1.resize(n); gameState2.resize(n);
    for (size_t i = 0; i < n; i++) {
        gameState1[i].resize(k);
        gameState2[i].resize(k);
        for (int j = 0; j < k; j++) {
            gameState1[i][j] = board1.board[i][j].first;
            gameState2[i][j] = board2.board[i][j].first;
        }
    }

    // Prepare the graph
    Graph graph;
    std::unordered_map<std::string, int> partition;
    for (size_t i = 0; i < n; i++) {
        std::string a = "A" + std::to_string(i);
        std::string b = "B" + std::to_string(i);
        partition[a] = 0; partition[b] = 1;
        graph.addVertex(a); graph.addVertex(b);
    }

    // Find perfect matching
    if (purpose != Purpose::GREATER) {
        // Populate graph with edges
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                switch (compareSortedCols(gameState1[i], gameState2[j])) {
                    case CompResult::LESS:
                    case CompResult::EQUAL: {
                        std::string a = "A" + std::to_string(i);
                        std::string b = "B" + std::to_string(j);
                        graph.addEdge(a, b); graph.addEdge(b, a);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        firstIsLess = hopcroftKarp(graph, partition) == n;
    }
    if (purpose != Purpose::LESS) {
        graph.clearEdges();
        // Populate graph with edges
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                switch (compareSortedCols(gameState1[i], gameState2[j])) {
                    case CompResult::GREATER:
                    case CompResult::EQUAL: {
                        std::string a = "A" + std::to_string(i);
                        std::string b = "B" + std::to_string(j);
                        graph.addEdge(a, b); graph.addEdge(b, a);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        secondIsLess = hopcroftKarp(graph, partition) == n;
    }

    if (firstIsLess && secondIsLess) return CompResult::EQUAL;
    else if (firstIsLess) return CompResult::LESS;
    else if (secondIsLess) return CompResult::GREATER;
    else return CompResult::INCOMPARABLE;
}

#else

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
        board1bst.push_back(board1.board[i][k - 1].first);
        board2bst.push_back(board2.board[i][k - 1].first);
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
                int item1 = board1.board[i][item].first;
                int item2 = board2.board[j][item].first;
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

#endif
