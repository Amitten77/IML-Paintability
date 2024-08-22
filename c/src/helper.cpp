#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include "../include/Board.h"
#include "../include/compare.h"
#include "../include/helper.h"

const int SCALE_FACTOR = 4;

void saveBoardsToFile(const std::vector<Board>& boards, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    for (const auto& board : boards) {
        outFile << board.serialize();
    }

    outFile.close();
}

void loadBoardsFromFile(const std::string& filename, std::vector<Board>& boards, int n, int k, int goal) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Error: Failed to open file for reading: " << filename << std::endl;
        return;
    }

    std::string line;

    while (getline(inFile, line)) {
        if (!line.empty()) {
            boards.emplace_back(line, n, k, goal);
        }
    }

    inFile.close();
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
                    case CompResult::LESS:
                        not_include[i] = true;
                        break;
                    case CompResult::GREATER:
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
    int threeCount = 0;
    for (int i = 0; i < board.n; i++) {
        int currTokens = 0;
        int tempMax = 0;
        for (int j = 0; j < board.k; j++) {
            const int index = board.get_index(i, j);
            if (board.board[index] != -1) {
                currTokens++;
                tempMax = std::max(tempMax, board.board[index]);
            }
        }
        if (currTokens == 3) {
            threeCount += 1;
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
    if (maxTokens == 2) {
        if (rowsFilled[0] + int(rowsFilled.size()) - 1 < board.goal) {
            return "LOSING";
        }
    }
    if (maxTokens == 3 && threeCount >= 3) {
        if (rowsFilled[0] + static_cast<int>(rowsFilled.size() * (3.0 / 2.0)) - 1 < board.goal) {
            return "LOSING";
        } 
    }
    // int temp = 0;
    auto it = LOSING.begin();
    for (const auto& lose_board : LOSING) {
        switch (compareBoards(board, lose_board, Purpose::LESS)) {
            case CompResult::LESS:
            case CompResult::EQUAL:
                // freq_count += 1;
                // freq_total += temp;
                if (it != LOSING.begin()) {
                    std::rotate(LOSING.begin(), it, std::next(it));
                }
                return "LOSING";
            default:
                // temp += 1;
                break;
        }
        ++it;
    }

    it = WINNING.begin();
    for (const auto& win_board : WINNING) {
        switch(compareBoards(board, win_board, Purpose::GREATER)) {
            case CompResult::GREATER:
            case CompResult::EQUAL:
                if (it != WINNING.begin()) {
                        std::rotate(WINNING.begin(), it, std::next(it));
                }
                return "WINNING";
            default:
                break;
        }
        ++it;
    }

    return "UNSURE";
}


int negaMax(Board& board, bool isPusher, int alpha, int beta, int depth) {
    if (isPusher) {
      //  board_total += 1;
        if (board.game_over()) {
         //   pruned_total += 1;
            if (board.max_score >= board.goal) {
                return board.goal;
            }
            return -1;
        }
        std::string status = checkStatus(board);
        if (status == "LOSING") {
            return -1;
        }
        if (status == "WINNING") {
            return board.goal;
        }
    }
    int bestVal = INT_MIN;
    if (isPusher) {
        std::vector<std::vector<int>> game_states = board.is_possible_push();
        for (size_t index = 0; index < game_states.size(); ++index) {
            Board nex(board);
            std::vector<int>& subset = game_states[index];
            nex.make_pusher_board(subset);
            int value = -negaMax(nex, !isPusher, -beta, -alpha, depth + 1);
            bestVal = std::max(bestVal, value);
            alpha = std::max(alpha, bestVal);
            if (depth < 1) {
                std::cout << std::fixed << std::setprecision(2) 
            << (((int)index + 1) * 100.0 / (int)game_states.size()) << "% done with the Game States for Depth "
            << depth << " for Possibility " << (index + 1) << " out of " << game_states.size() << '\n';
            }
            if (bestVal >= board.goal || beta <= alpha) {
                break;
            } 
        }
    } else if (!isPusher) {
        std::vector<int> game_states = board.is_possible_remove();
        for (size_t index = 0; index < game_states.size(); ++index) {
            int poss = game_states[index];
            Board nex(board);
            nex.make_remover_board(poss);
            int value = -negaMax(nex, !isPusher, -beta, -alpha, depth + 1);
            bestVal = std::max(bestVal, value);
            alpha = std::max(alpha, bestVal);
            if (bestVal == 1 || beta <= alpha) {
                break;
            } 
        }
    }
    if (isPusher) {
        if (bestVal < board.goal) {
            LOSING.push_back(board);
            if ((int)LOSING.size() > LOSING_BOUND) {
                std::cout << "Losing Length Before Pruning: " << LOSING.size() << '\n';
                prune_losing();
                std::cout << "Losing Length After Pruning: " << LOSING.size() << '\n';
                LOSING_BOUND = std::max(LOSING_BOUND, int((LOSING.size() * SCALE_FACTOR)));
            } 
        } else {
            WINNING.push_back(board);
            if ((int)WINNING.size() > WINNING_BOUND) {
                std::cout << "Winning Length Before Pruning: " << WINNING.size() << '\n';
                prune_winning();
                std::cout << "Winning Length After Pruning: " << WINNING.size() << '\n';
                WINNING_BOUND = std::max(WINNING_BOUND, int((WINNING.size() * SCALE_FACTOR)));
            }
        }
    }
    return bestVal;
 }


