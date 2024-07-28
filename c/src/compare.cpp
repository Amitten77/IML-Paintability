#include <algorithm>
#include <functional>
#include "compare.h"
#include "graph.h"

#define USE_HOPCROFT_KARP

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
    // 1. Make sure the boards have the same dimensions
    if (board1.getN() != board2.getN() || board1.getK() != board2.getK()) {
        return CompResult::INCOMPARABLE;
    }
    size_t n = board1.getN();
    size_t k = board1.getK();

    // 2. Edge cases
    if (n == 0 || k == 0) {
        return CompResult::EQUAL;
    }

    bool possLess = true;  // Whether board 1 is possible to be less than board 2
    bool possMore = true;  // Whether board 2 is possible to be more than board 1
    if (purpose == Purpose::LESS) {
        possMore = false;
    }
    if (purpose == Purpose::GREATER) {
        possLess = false;
    }
    if (board1.getNumChips() > board2.getNumChips()) {
        possLess = false;
    }
    if (board1.getNumChips() < board2.getNumChips()) {
        possMore = false;
    }
    if (!possLess && !possMore) {
        return CompResult::INCOMPARABLE;
    }

    // 3. Check the highest chip in each column. If they are incomparable then the entire boards are incomparable.
    std::vector<int> board1Top;
    std::vector<int> board2Top;
    board1Top.reserve(n);
    board2Top.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        board1Top.push_back(board1.getBoardState().at(i).at(0));
        board2Top.push_back(board2.getBoardState().at(i).at(0));
    }
    std::sort(board1Top.begin(), board1Top.end(), std::greater<>());
    std::sort(board2Top.begin(), board2Top.end(), std::greater<>());
    if (compareSortedCols(board1Top, board2Top) == CompResult::INCOMPARABLE) {
        return CompResult::INCOMPARABLE;
    }

    // 4. Actual comparison algorithm. See paper for details
    bool firstIsLess = false, secondIsLess = false;

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
                switch (compareSortedCols(board1.getBoardState().at(i), board2.getBoardState().at(j))) {
                    case CompResult::LESS:
                    case CompResult::EQUAL: {
                        std::string a = "A" + std::to_string(i);
                        std::string b = "B" + std::to_string(j);
                        graph.addEdge(a, b);
                        graph.addEdge(b, a);
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
                switch (compareSortedCols(board1.getBoardState().at(i), board2.getBoardState().at(j))) {
                    case CompResult::GREATER:
                    case CompResult::EQUAL: {
                        std::string a = "A" + std::to_string(i);
                        std::string b = "B" + std::to_string(j);
                        graph.addEdge(a, b);
                        graph.addEdge(b, a);
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
