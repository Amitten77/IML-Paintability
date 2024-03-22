#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include "../include/Board.h"
#include "../include/compare.h"
#include "../include/helper.h"

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
    std::vector<bool> not_include(LOSING.size(), false);
    for (size_t i = 0; i < LOSING.size(); i++) {
        for (size_t j = std::max(i + 1, PREV_LOSING); j < LOSING.size(); j++) {
            if (!not_include[i] && !not_include[j]) {
                switch(compareBoards(LOSING[i], LOSING[j])) {
                    case CompResult::EQUAL:
                        not_include[i] = true;
                        break;
                    case CompResult::GREATER:
                        not_include[i] = true;
                        break;
                    case CompResult::LESS:
                        not_include[j] = true;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    std::vector<Board> pruned;
    pruned.reserve(LOSING.size() - not_include.size());
    for (size_t i = 0; i < LOSING.size(); ++i) {
        if (!not_include[i]) {
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
            if (!not_include[i] && !not_include[j]) {
                switch(compareBoards(WINNING[i], WINNING[j])) {
                    case CompResult::EQUAL:
                        not_include[i] = true;
                        break;
                    case CompResult::GREATER:
                        not_include[i] = true;
                        break;
                    case CompResult::LESS:
                        not_include[j] = true;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    std::vector<Board> pruned;
    pruned.reserve(WINNING.size() - not_include.size());
    for (size_t i = 0; i < WINNING.size(); ++i) {
        if (!not_include[i]) {
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
        if (rowsFilled[index] + (int)index >= board.goal) {
            return "WINNING";
        }
    }

    if (maxTokens == 1) {
        return "LOSING";
    }

    for (const auto& lose_board : LOSING) {
        switch (compareBoards(board, lose_board, Purpose::LESS)) {
            case CompResult::GREATER:
            case CompResult::EQUAL:
                return "LOSING";
            case CompResult::LESS:
                return "LOSING";
            default:
                break;
        }
    }

    for (const auto& win_board : WINNING) {
        switch(compareBoards(board, win_board, Purpose::GREATER)) {
            case CompResult::LESS:
            case CompResult::EQUAL:
                return "WINNING";
            case CompResult::GREATER:
                return "WINNING";
            default:
                break;
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
                LOSING_BOUND = std::max(LOSING_BOUND, int((LOSING.size() * SCALE_FACTOR)));
            }
        } else {
            WINNING.push_back(board);
            if ((int)WINNING.size() > WINNING_BOUND) {
                std::cout << "Winning Length Before Pruning: " << WINNING.size() << std::endl;
                prune_winning();
                std::cout << "Winning Length After Pruning: " << WINNING.size() << std::endl;
                WINNING_BOUND = std::max(WINNING_BOUND, int((WINNING.size() * SCALE_FACTOR)));
            }
        }
    }
    return bestVal;
}
