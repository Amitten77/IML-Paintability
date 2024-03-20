#include <functional>
#include "helper.h"
#include "Board.h"
#include "graph.h"

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

int compareSortedCols(const std::vector<int>& col1, const std::vector<int>& col2) {
    size_t n = std::max(col1.size(), col2.size());
    bool col1IsLess = true, col2IsLess = true;

    for (size_t i = 0; i < n; i++) {
        // Compare i-th element
        int num1 = (i >= col1.size()) ? -1 : col1[i];
        int num2 = (i >= col2.size()) ? -1 : col2[i];
        if (num1 > num2) col1IsLess = false;
        if (num2 > num1) col2IsLess = false;
    }

    if (col1IsLess && col2IsLess) return 0;
    else if (col1IsLess) return -1;
    else if (col2IsLess) return 1;
    else return 2;
}

//int lessThan(const Board& board1, const Board& board2, const std::string& purpose) {
//    if (board1.n != board2.n || board1.k != board2.k) {
//        return 2;
//    }
//    int n = board1.n;
//    int k = board1.k;
//
//    if (purpose == "LESS" || purpose == "GREATER") {
//        if (purpose == "LESS" && board1.num_tokens > board2.num_tokens) {
//            return 2;
//        }
//
//        if (purpose == "GREATER" && board1.num_tokens < board2.num_tokens) {
//            return 2;
//        }
//        std::vector<int> board1bst;
//        std::vector<int> board2bst;
//        for (int i = 0; i < n; ++i) {
//            board1bst.push_back(board1.board[i][k - 1].first);
//            board2bst.push_back(board2.board[i][k - 1].first);
//        }
//        std::sort(board1bst.begin(), board1bst.end());
//        std::sort(board2bst.begin(), board2bst.end());
//        for (int i = 0; i < n; ++i) {
//            if (purpose == "LESS" && board1bst[i] > board2bst[i]) {
//                return 2;
//            }
//            if (purpose == "GREATER" && board1bst[i] < board2bst[i]) {
//                return 2;
//            }
//        }
//    }
//
//    std::vector<std::unordered_set<int>> lessThanRelation(n), greaterThanRelation(n);
//
//
//
//    for (int i = 0; i < n; ++i) {
//        for (int j = 0; j < n; ++j) {
//            bool less = true, greater = true;
//            for (int item = 0; item < board1.k; ++item) {
//                int item1 = board1.board[i][item].first;
//                int item2 = board2.board[j][item].first;
//                if (item1 > item2) less = false;
//                if (item1 < item2) greater = false;
//                if (!greater && !less) break;
//            }
//            if (less) lessThanRelation[i].insert(j);
//            if (greater) greaterThanRelation[i].insert(j);
//        }
//    }
//
//    // Check Hall's condition for both relations
//    bool isMore = false;
//    bool isLess = false;
//    if (purpose == "GREATER") {
//        isMore = checkHallsCondition(greaterThanRelation, n);
//    } else if (purpose == "LESS") {
//        isLess = checkHallsCondition(lessThanRelation, n);
//    } else {
//        isMore = checkHallsCondition(greaterThanRelation, n);
//        isLess = checkHallsCondition(lessThanRelation, n);
//    }
//    if (isMore && isLess) return 0;
//    if (isLess) return 1;
//    if (isMore) return -1;
//    return 2;
//}

int lessThan(const Board& board1, const Board& board2, const std::string& purpose) {
    if (board1.n != board2.n || board1.k != board2.k) {
        return 2;
    }
    size_t n = board1.n;
    int k = board1.k;

    if (purpose == "LESS" || purpose == "GREATER") {
        if (purpose == "LESS" && board1.num_tokens > board2.num_tokens) return 2;
        if (purpose == "GREATER" && board1.num_tokens < board2.num_tokens) return 2;

        // Highest row?
        std::vector<int> board1bst, board2bst;
        board1bst.reserve(n);
        board2bst.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            board1bst.push_back(board1.board[i][k - 1].first);
            board2bst.push_back(board2.board[i][k - 1].first);
        }

        //
        std::sort(board1bst.begin(), board1bst.end());
        std::sort(board2bst.begin(), board2bst.end());
        for (size_t i = 0; i < n; ++i) {
            if (purpose == "LESS" && board1bst[i] > board2bst[i]) return 2;
            if (purpose == "GREATER" && board1bst[i] < board2bst[i]) return 2;
        }
    }

    bool firstIsLess = true, secondIsLess = true;
    // Represent the boards in the standard notation
    // TODO
    std::vector<std::vector<int>> board1Std, board2Std;

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
    if (purpose != "GREATER") {
        // Populate g with edges
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                int result = compareSortedCols(board1Std[i], board2Std[j]);
                if (result == -1 || result == 0) {
                    std::string a = "A" + std::to_string(i);
                    std::string b = "B" + std::to_string(j);
                    graph.addEdge(a, b); graph.addEdge(b, a);
                }
            }
        }
        firstIsLess = hopcroftKarp(graph, partition) == n;
    }
    if (purpose != "LESS") {
        // Populate g with edges
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                int result = compareSortedCols(board1Std[i], board2Std[j]);
                if (result == 1 || result == 0) {
                    std::string a = "A" + std::to_string(i);
                    std::string b = "B" + std::to_string(j);
                    graph.addEdge(a, b); graph.addEdge(b, a);
                }
            }
        }
        secondIsLess = hopcroftKarp(graph, partition) == n;
    }

    if (firstIsLess && secondIsLess) return 0;
    else if (firstIsLess) return -1;
    else if (secondIsLess) return 1;
    else return 2;
}

std::vector<std::vector<int>> product(const std::vector<std::vector<std::vector<int>>>& lists) {
    std::vector<std::vector<int>> result;

    for (const auto& list : lists) {
        if (list.empty()) return {{}};
    }
    result.emplace_back();

    for (const auto& list : lists) {
        std::vector<std::vector<int>> tempResult;

        for (const auto& existingCombo : result) {
            for (const auto& item : list) {
                std::vector<int> newCombo = existingCombo;
                newCombo.insert(newCombo.end(), item.begin(), item.end());
                tempResult.push_back(newCombo);
            }
        }

        result = std::move(tempResult);
    }

    return result;
}


void generateCombinations(std::vector<std::vector<int>>& subsets, std::vector<int>& subset, int start, int n, int k) {
    if (k == 0) {
        subsets.push_back(subset);
        return;
    }
    for (int i = start; i <= n - k; ++i) {
        subset.push_back(i);
        generateCombinations(subsets, subset, i + 1, n, k - 1);
        subset.pop_back();
    }
}

void initMap(int N, int K) {
    std::vector<int> values(N * K);
    std::iota(values.begin(), values.end(), 0);

    std::vector<std::vector<int>> subsets;
    std::vector<int> subset;
    int index = 0;

    for (int i = 0; i <= N * K; ++i) {
        generateCombinations(subsets, subset, 0, N * K, i);
    }

    for (const auto& s : subsets) {
        subset_graph[index] = s;
        num_graph[s] = index;
        ++index;
    }
}

void prune_losing() {
    std::unordered_set<int> not_include;
    for (size_t i = 0; i < LOSING.size(); i++) {
        for (size_t j = i + 1; j < LOSING.size(); j++) {
            int score = lessThan(LOSING[i], LOSING[j]);
            if (score == 0 || score == 1) {
                not_include.insert((int)i);
            }
            if (score == -1) {
                not_include.insert((int)j);
            }
        }
    }
    std::vector<Board> pruned;
    pruned.reserve(LOSING.size() - not_include.size());
    for (size_t i = 0; i < LOSING.size(); ++i) {
        if (not_include.find((int)i) == not_include.end()) {
            pruned.push_back(std::move(LOSING[i]));
        }
    }
    
    LOSING = std::move(pruned);
}

void prune_winning() {
    std::unordered_set<int> not_include;
    for (size_t i = 0; i < WINNING.size(); i++) {
        for (size_t j = i + 1; j < WINNING.size(); j++) {
            int score = lessThan(WINNING[i], WINNING[j]);
            if (score == 0 || score == -1) {
                not_include.insert((int)i);
            }
            if (score == 1) {
                not_include.insert((int)j);
            }
        }
    }

    std::vector<Board> pruned;
    pruned.reserve(WINNING.size() - not_include.size());
    for (size_t i = 0; i < WINNING.size(); ++i) {
        if (not_include.find((int)i) == not_include.end()) {
            pruned.push_back(std::move(WINNING[i]));
        }
    }

    WINNING = std::move(pruned);
}

std::string checkStatus(const Board &board) {
    int maxTokens = 0;
    std::vector<int> rowsFilled;

    for (const auto& row : board.board) {
        int currTokens = 0;
        int tempMax = 0;
        for (const auto& item : row) {
            if (item.first != -1) {
                currTokens++;
                tempMax = std::max(tempMax, item.first);
            }
        }
        maxTokens = std::max(maxTokens, currTokens);
        if (currTokens > 0) {
            rowsFilled.push_back(tempMax);
        }
    }

    std::sort(rowsFilled.begin(), rowsFilled.end(), std::greater<int>());

    for (size_t index = 0; index < rowsFilled.size(); ++index) {
        if (rowsFilled[index] + (int)index >= board.goal) {
            return "WINNING";
        }
    }

    if (maxTokens == 1) {
        return "LOSING";
    }

    for (const auto& lose_board : LOSING) {
        int temp = lessThan(board, lose_board, "LESS");
        if (temp == 1 || temp == 0) {
            return "LOSING";
        }
    }

    for (const auto& win_board : WINNING) {
        int temp = lessThan(board, win_board, "GREATER");
        if (temp == 0 || temp == -1) {
            return "WINNING";
        }
    }

    return "UNSURE";
}

int negaMax(Board& board, bool isPusher, int alpha, int beta, int depth) {
    int multiplier = -1;
    if (isPusher) {
        multiplier = 1;
    }
    if (isPusher) {
        if (board.game_over()) {
            if (board.max_score < board.goal) {
                LOSING.push_back(board);
                if ((int)LOSING.size() > LOSING_BOUND) {
                    std::cout << "Losing Length Before Pruning: " << LOSING.size() << std::endl;
                    prune_losing();
                    std::cout << "Losing Length After Pruning: " << LOSING.size() << std::endl;
                    LOSING_BOUND = std::max(LOSING_BOUND, int(LOSING.size()) + 40);
                }
            }
            return board.max_score * multiplier;
        }
        std::string status = checkStatus(board);
        if (status == "LOSING") {
            return -1 * multiplier;
        }
        if (status == "WINNING") {
            return board.goal * multiplier;
        }
    }
    int bestVal = INT_MIN;
    std::vector<int> game_states = isPusher ? board.is_possible_push() : board.is_possible_remove();

    std::reverse(game_states.begin(), game_states.end());

    for (size_t index = 0; index < game_states.size(); ++index) {
        int poss = game_states[index];
        Board nex(board);
        if (isPusher) {
            std::vector<int> subset = subset_graph[poss];
            nex.make_pusher_board(subset);
        } else {
            nex.make_remover_board(poss);
        }
        int value = -negaMax(nex, !isPusher, -beta, -alpha, depth + 1);
        bestVal = std::max(bestVal, value);
        alpha = std::max(alpha, bestVal);
        if (isPusher && depth < 3) {
            std::cout << std::fixed << std::setprecision(2) 
          << (((int)index + 1) * 100.0 / (int)game_states.size()) << "% done with the Game States for Depth "
          << depth << " for Possibility " << (index + 1) << " out of " << game_states.size() << std::endl;
        }
        if (!isPusher && bestVal == 1) {
            break;
        }
        if (isPusher && bestVal > board.goal) {
            break;
        }
        if (bestVal >= board.goal || beta <= alpha) {
            break;
        } 
    }
    if (isPusher) {
        if (bestVal < board.goal) {
            LOSING.push_back(board);
            if ((int)LOSING.size() > LOSING_BOUND) {
                std::cout << "Losing Length Before Pruning: " << LOSING.size() << std::endl;
                prune_losing();
                std::cout << "Losing Length After Pruning: " << LOSING.size() << std::endl;
                LOSING_BOUND = std::max(LOSING_BOUND, int(LOSING.size()) + 40);
            }
        } else {
            WINNING.push_back(board);
            if ((int)WINNING.size() > WINNING_BOUND) {
                std::cout << "Winning Length Before Pruning: " << WINNING.size() << std::endl;
                prune_winning();
                std::cout << "Winning Length After Pruning: " << WINNING.size() << std::endl;
                WINNING_BOUND = std::max(WINNING_BOUND, int(WINNING.size()) + 40);
            }
        }
    }
    return bestVal;
}
