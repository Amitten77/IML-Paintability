#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <queue>
#include <sstream>
#include <stack>
#include "../include/Board.h"
#include "../include/compare.h"
#include "../include/helper.h"
#include "../include/board_operation.h"

//#define USE_NEW_NEGAMAX
//#define RECORD_PARTIAL_RESULT

const int SCALE_FACTOR = 3;

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
            case CompResult::LESS:
            case CompResult::EQUAL:
                return "LOSING";
            default:
                break;
        }
    }

    for (const auto& win_board : WINNING) {
        switch(compareBoards(board, win_board, Purpose::GREATER)) {
            case CompResult::GREATER:
            case CompResult::EQUAL:
                return "WINNING";
            default:
                break;
        }
    }

    return "UNSURE";
}

#ifdef USE_NEW_NEGAMAX

int negaMax(Board& board, bool isPusher, int alpha, int beta, int depth) {
    (void)isPusher, (void)alpha, (void)beta, (void)depth;

    // First check if the beginning state is already in the winning/loosing state
    switch (checkWinner(board)) {
        case Player::PUSHER:
            printf("Pusher wins with score %d\n", board.max_score);
            return board.max_score;
        case Player::REMOVER:
            printf("Pusher loses with score %d\n", board.max_score);
            return board.max_score;
        default:
            if (boardIsWinning(board, WINNING)) {
                printf("Pusher predicted to win\n");
            } else if (boardIsLosing(board, LOSING)) {
                printf("Pusher predicted to lose\n");
            }
            break;
    }

    // <Current board, isPusher, children>
    std::stack<std::tuple<Board, std::vector<Board>, std::vector<Player>>> boards;
    // todo: find children
    boards.emplace(board, std::vector<Board>());

    while (!boards.empty()) {
        auto [currState, nextStates, nextWinners] = boards.top();
        // printf("%s\n", curr.serialize().c_str());

        // Check if any next state is not categorized into winning or losing
        std::vector<Board> toChecks;
        for (const Board& afterPusher : nextStates) {
            stepRemover(afterPusher, toChecks, [](const Board& item) {
                return !boardIsWinning(item, WINNING) && !boardIsLosing(item, LOSING);
            });
        }

        if (toChecks.empty()) {
            // If all pushers
        }
    }
}

#else

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

    std::vector<int> game_states;
    if (isPusher) {
        std::vector<PusherMove> pusherMoves;
        getAllPusherMovesPruned(board, pusherMoves);
        for (PusherMove& move : pusherMoves) {
            std::sort(move.begin(), move.end());
            game_states.push_back(num_graph[move]);
        }
    } else {
        game_states = board.is_possible_remove();
    }

    // std::reverse(game_states.begin(), game_states.end());

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
                // Output current winning and losing boards

#ifdef RECORD_PARTIAL_RESULT
                if (LOSING.size() >= 1000) {
                    prune_winning();

                    std::stringstream currentTime;
                    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    std::tm buf;
                    localtime_s(&buf, &now);
                    currentTime << std::put_time(&buf, "%Y-%m-%d_%H-%M");
                    std::string FILE_NAME = "N" + std::to_string(board.n)
                                            + "_K" + std::to_string(board.k)
                                            + "_goal" + std::to_string(board.goal)
                                            + "_" + currentTime.str() + ".txt";
                    std::string WINNING_FILE = "winning/" + FILE_NAME;
                    std::string LOSING_FILE = "losing/" + FILE_NAME;
                    saveBoardsToFile(WINNING, WINNING_FILE);
                    saveBoardsToFile(LOSING, LOSING_FILE);
                }
#endif
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

#endif
