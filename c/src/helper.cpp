#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include "../include/Board.h"
#include "../include/compare.h"
#include "../include/helper.h"

const int SCALE_FACTOR = 3;

// void saveBoardsToFile(const std::vector<Board>& boards, const std::string& filename) {
//     std::ofstream file(filename);
//     if (!file.is_open()) {
//         std::cerr << "Failed to open file for writing: " << filename << std::endl;
//         return;
//     }

//     for (const Board& board : boards) {
//         file << board.serialize();
//         file << "---\n"; // Use a delimiter to separate boards
//     }

//     file.close();
// }

// void loadBoardsFromFile(const std::string& filename, std::vector<Board>& boards) {
//     std::ifstream file(filename);
//     if (!file.is_open()) {
//         std::cerr << "Failed to open file for reading: " << filename << std::endl;
//         return;
//     }

//     std::string line;
//     std::string serializedBoard;
//     while (std::getline(file, line)) {
//         if (line == "---") { // Board delimiter
//             if (!serializedBoard.empty()) {
//                 boards.emplace_back(serializedBoard);
//                 serializedBoard.clear();
//             }
//         } else {
//             serializedBoard += line + "\n";
//         }
//     }
    
//     // Don't forget to add the last board if the file doesn't end with "---"
//     if (!serializedBoard.empty()) {
//         boards.emplace_back(serializedBoard);
//     }
// }

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
        if (rowsFilled[0] + rowsFilled.size() - 1 < board.goal) {
            return "LOSING";
        }
    }
    if (maxTokens == 3) {
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
        if (board.game_over()) {
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
            << depth << " for Possibility " << (index + 1) << " out of " << game_states.size() << std::endl;
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

// int PVS(Board& board, bool isPusher, int alpha, int beta, int depth) {
//     if (isPusher) {
//         if (board.game_over()) {
//             if (board.max_score >= board.goal) {
//                 return board.goal;
//             }
//             return -1;
//         }
//         std::string status = checkStatus(board);
//         if (status == "LOSING") {
//             return -1;
//         }
//         if (status == "WINNING") {
//             return board.goal;
//         }
//     }
//     int bestVal = INT_MIN;
//     if (isPusher) {
//         std::vector<std::vector<int>> game_states = board.is_possible_push();
//         for (size_t index = 0; index < game_states.size(); ++index) {
//             Board nex(board);
//             std::vector<int>& subset = game_states[index];
//             nex.make_pusher_board(subset);
//             int value = -1;
//             if (index == 0) {
//                 value = -PVS(nex, !isPusher, -beta, -alpha, depth + 1);
//             } else {
//                 value = -PVS(nex, !isPusher, -alpha - 1, -alpha, depth + 1);
//                 if (alpha < value < beta) {
//                     Board nex2(board);
//                     std::vector<int>& subset = game_states[index];
//                     nex2.make_pusher_board(subset);
//                     value = -PVS(nex, !isPusher, -beta, -alpha, depth + 1);
//                 }
//             }
//             bestVal = std::max(bestVal, value);
//             alpha = std::max(alpha, bestVal);
//             if (depth < 3) {
//                 std::cout << std::fixed << std::setprecision(2) 
//             << (((int)index + 1) * 100.0 / (int)game_states.size()) << "% done with the Game States for Depth "
//             << depth << " for Possibility " << (index + 1) << " out of " << game_states.size() << std::endl;
//             }
//             if (bestVal >= board.goal || beta <= alpha) {
//                 break;
//             } 
//         }
//     } else if (!isPusher) {
//         std::vector<int> game_states = board.is_possible_remove();
//         for (size_t index = 0; index < game_states.size(); ++index) {
//             int poss = game_states[index];
//             Board nex(board);
//             nex.make_remover_board(poss);
//             int value = -1;
//             if (index == 0) {
//                 value = -PVS(nex, !isPusher, -beta, -alpha, depth + 1);
//             } else {
//                 value = -PVS(nex, !isPusher, -alpha - 1, -alpha, depth + 1);
//                 if (alpha < value < beta) {
//                     Board nex2(board);
//                     int subset = game_states[index];
//                     nex2.make_remover_board(subset);
//                     value = -PVS(nex, !isPusher, -beta, -alpha, depth + 1);
//                 }
//             }
//             bestVal = std::max(bestVal, value);
//             alpha = std::max(alpha, bestVal);
//             if (bestVal == 1 || beta <= alpha) {
//                 break;
//             } 
//         }
//     }
//     if (isPusher) {
//         if (bestVal < board.goal) {
//             LOSING.push_back(board);
//             if ((int)LOSING.size() > LOSING_BOUND) {
//                 std::cout << "Losing Length Before Pruning: " << LOSING.size() << std::endl;
//                 prune_losing();
//                 std::cout << "Losing Length After Pruning: " << LOSING.size() << std::endl;
//                 LOSING_BOUND = std::max(LOSING_BOUND, int((LOSING.size() * SCALE_FACTOR)));
//             } 
//         } else {
//             WINNING.push_back(board);
//             if ((int)WINNING.size() > WINNING_BOUND) {
//                 std::cout << "Winning Length Before Pruning: " << WINNING.size() << std::endl;
//                 prune_winning();
//                 std::cout << "Winning Length After Pruning: " << WINNING.size() << std::endl;
//                 WINNING_BOUND = std::max(WINNING_BOUND, int((WINNING.size() * SCALE_FACTOR)));
//             }
//         }
//     }
//     return bestVal;
// }


// int MTDF(Board& board, int guess) {
//     int upperBound = INT_MAX;
//     int lowerBound = INT_MIN;
//     int beta = -1;
//     while (lowerBound < upperBound) {
//         beta = std::max(guess, lowerBound + 1);
//         Board nex(board);
//         std::cout << beta - 1 << " " << beta;
//         guess = negaMax(nex, true, beta - 1, beta, 0);
//         if (guess < beta) {
//             upperBound = guess;
//         } else {
//             lowerBound = guess;
//         }
//     }
//     return guess;
// }


