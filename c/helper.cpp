#include "helper.h"
#include "Board.h"


const int SCALE_FACTOR = 2;

void saveBoardsToFile(const std::vector<Board>& boards, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    for (const Board& board : boards) {
        file << board.serialize();
        file << "---\n"; // Use a delimiter to separate boards
    }

    file.close();
}

void loadBoardsFromFile(const std::string& filename, std::vector<Board>& boards) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return;
    }

    std::string line;
    std::string serializedBoard;
    while (std::getline(file, line)) {
        if (line == "---") { // Board delimiter
            if (!serializedBoard.empty()) {
                boards.emplace_back(serializedBoard);
                serializedBoard.clear();
            }
        } else {
            serializedBoard += line + "\n";
        }
    }
    
    // Don't forget to add the last board if the file doesn't end with "---"
    if (!serializedBoard.empty()) {
        boards.emplace_back(serializedBoard);
    }
}



int checkHallsCondition(const std::vector<std::unordered_set<int>>& relation, 
  int n) {
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


int lessThan(const Board& board1, const Board& board2, const std::string& purpose) {
    if (board1.n != board2.n || board1.k != board2.k) {
        return 2;
    }
    int n = board1.n;
    int k = board1.k;
    bool possLess = true;
    bool possMore = true;
    if (purpose == "LESS") {
        possMore = false;
    }
    if (purpose == "GREATER") {
        possLess = false;
    }
    if (board1.num_tokens > board2.num_tokens) {
        possLess = false;
    }

    if (board1.num_tokens < board2.num_tokens) {
        possMore = false;
    }
    if (!possLess && !possMore) {
        return 2;
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
            return 2;
        }
    }

    std::vector<std::unordered_set<int>> lessThanRelation(n),  greaterThanRelation(n);

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
            if (less) {
                lessThanRelation[i].insert(j);
            }
            if (greater) { 
                greaterThanRelation[i].insert(j);
            }
        }
        if (lessThanRelation[i].size() == 0) {
            possLess = false;
        }
        if (purpose == "GREATER" && greaterThanRelation[i].size() == 0) {
            possMore = false;
        }
        if (!possLess && !possMore) {
            return 2;
        }
    }
    if (!possLess && !possMore) {
            return 2;
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
    if (isMore && isLess) return 0;
    if (isLess) return 1;
    if (isMore) return -1;
    return 2;
}

std::vector<std::vector<int>> product(const std::vector<std::vector<std::vector<int>>>& lists) {
    std::vector<std::vector<int>> result;

    for (const auto& list : lists) {
        if (list.empty()) return {{}};
    }
    result.push_back({});

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
    std::vector<bool> not_include(LOSING.size(), false);
    for (size_t i = 0; i < LOSING.size(); i++) {
        for (size_t j = std::max(i + 1, PREV_LOSING); j < LOSING.size(); j++) {
            if (not_include[i] == false && not_include[j] == false) {
                int score = lessThan(LOSING[i], LOSING[j]);
                if (score == 0 || score == 1) {
                    not_include[i] = true;
                }
                if (score == -1) {
                    not_include[j] = true;
                }
            }
        }
    }
    std::vector<Board> pruned;
    pruned.reserve(LOSING.size() - not_include.size());
    for (size_t i = 0; i < LOSING.size(); ++i) {
        if (not_include[i] == false) {
            pruned.push_back(std::move(LOSING[i]));
        }
    }
    
    LOSING = std::move(pruned);
    PREV_LOSING = LOSING.size();
}

void prune_winning() {
    std::vector<bool> not_include(WINNING.size(), false);
    for (size_t i = 0; i < WINNING.size(); i++) {
        for (size_t j = std::max(i + 1, PREV_WINNING); j < WINNING.size(); j++) {
            if (not_include[i] == false && not_include[j] == false) {
                int score = lessThan(WINNING[i], WINNING[j]);
                if (score == 0 || score == 1) {
                    not_include[i] = true;
                }
                if (score == -1) {
                    not_include[j] = true;
                }
            }
        }
    }

    std::vector<Board> pruned;
    pruned.reserve(WINNING.size() - not_include.size());
    for (size_t i = 0; i < WINNING.size(); ++i) {
        if (not_include[i] == false) {
            pruned.push_back(std::move(WINNING[i]));
        }
    }

    WINNING = std::move(pruned);
    PREV_WINNING = WINNING.size();
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
            if (rowsFilled[index] + index >= board.goal) {
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
                if (LOSING.size() > LOSING_BOUND) {
                    std::cout << "Losing Length Before Pruning: " << LOSING.size() << std::endl;
                    prune_losing();
                    std::cout << "Losing Length After Pruning: " << LOSING.size() << std::endl;
                    LOSING_BOUND = std::max(LOSING_BOUND, int((LOSING.size() * SCALE_FACTOR)));
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
          << ((index + 1) * 100.0 / game_states.size()) << "% done with the Game States for Depth " 
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
            if (LOSING.size() > LOSING_BOUND) {
                std::cout << "Losing Length Before Pruning: " << LOSING.size() << std::endl;
                prune_losing();
                std::cout << "Losing Length After Pruning: " << LOSING.size() << std::endl;
                LOSING_BOUND = std::max(LOSING_BOUND, int((LOSING.size() * SCALE_FACTOR)));
            }
        } else {
            WINNING.push_back(board);
            if (WINNING.size() > WINNING_BOUND) {
                std::cout << "Winning Length Before Pruning: " << WINNING.size() << std::endl;
                prune_winning();
                std::cout << "Winning Length After Pruning: " << WINNING.size() << std::endl;
                WINNING_BOUND = std::max(WINNING_BOUND, int((WINNING.size() * SCALE_FACTOR)));
            }
        }
    }
    return bestVal;
}