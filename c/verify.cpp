/**
 * @file Verify if a board is winning or losing.
 *
 * According to the definition, a board is winning if it is greater than or equal to a board in the list of winning
 * states, and it is losing if it is less than or equal to a board in the list of losing states. By comparing to the
 * two list of game states, we can decide if the target board is 1) winning, 2) losing, or 3) unknown.
 *
 * That is assuming the two lists are indeed winning/losing states. The purpose of this program is to verify the
 * correctness of the two lists of states. The list of winning states must be valid if the following is verified:
 * a) Starting from any winning state, after a move each by the Pusher and the Remover consecutively, the Pusher can
 *    guarantee the resulting state is either already winning, or a winning state according to the above definition;
 * b) Starting from any losing state, after a move each by the Pusher and the Remover consecutively, the Remover can
 *    guarantee the resulting state is either already losing, or a losing state according to the above definition;
 */

#include <algorithm>
#include <atomic>
#include <fstream>
#include <future>
#include <mutex>
#include <queue>
#include <vector>
#include "json.hpp"
#include "Board.h"
#include "board_operation.h"
#include "compare.h"
#include "helper.h"
#include "init.h"

size_t verifyWinningStatesThread(const std::vector<Board>& winningStates, std::atomic<size_t>& counter, size_t logFreq) {
    size_t total = winningStates.size();
    size_t countUnverified = 0;

    while (true) {
        // Log progress
        size_t i = counter.fetch_add(1, std::memory_order_relaxed);
        if (i >= total) break;
        if ((i + 1) % logFreq == 0 || i + 1 == total) {
            printf("[Verify Winning] %zu / %zu\n", i + 1, total);
        }

        const Board& board = winningStates[i];

        // Get all pusher moves
        std::vector<PusherMove> pusherMoves;
#ifdef USE_PRUNED_MOVES_FOR_VERIFICATION
        getAllPusherMovesPruned(board, pusherMoves);
#else
        pusherMoves = getAllPusherMoves(board);
#endif
        bool canGuaranteeWinning = std::any_of(
                pusherMoves.begin(), pusherMoves.end(),
                [&winningStates, i](const PusherMove& pusherMove) {
                    // Apply pusher move
                    Board board1 = winningStates[i];
                    applyPusherMove(board1, pusherMove);

                    // Get all remover moves
                    std::vector<int> removerMoves;
                    getAllRemoverMovesPruned(board1, removerMoves);

                    // If all moves lead to Pusher victory or a confirmed winning state, then this is a winning state
                    return std::all_of(
                            removerMoves.begin(), removerMoves.end(),
                            [&winningStates, i, board1](const int removerMove) {
                                Board board2 = board1;
                                applyRemoverMove(board2, removerMove);

                                // Check if state is winning or this board is a confirmed "winning state"
                                if (checkStatus(board2) == "WINNING") return true;
                                return boardIsWinning(board2, winningStates.begin(), winningStates.begin() + i);
                            });
                });

        if (!canGuaranteeWinning) {
            countUnverified++;
            printf("The following board is not confirmed as a winning state:\n%s\n", board.serialize().c_str());
        }
    }

    return countUnverified;
}

size_t verifyWinningStates(const std::vector<Board>& winningStates, size_t j, size_t logFreq) {
    // Sort the winning states by their number of tokens
    std::vector<Board> winningStates_ = winningStates;
    std::sort(winningStates_.begin(), winningStates_.end(), [](const Board& board1, const Board& board2) {
        return board1.num_tokens < board2.num_tokens;
    });

    // Define variables
    size_t total = winningStates_.size();
    std::atomic<size_t> counter = 0;
    std::vector<std::future<size_t>> futures;
    futures.reserve(j);

    // Create threads
    for (size_t index = 0; index < j; index++) {
        futures.push_back(std::async(
                std::launch::async,
                verifyWinningStatesThread,
                std::ref(winningStates_), std::ref(counter), logFreq));
    }

    // Join threads
    size_t countUnverified = 0;
    for (auto& fut : futures) {
        countUnverified += fut.get();
    }

    // Print results
    printf("Verified all winning states.\n");
    if (countUnverified) {
        printf("%zu (out of %zu) states not confirmed.\n", countUnverified, total);
    } else {
        printf("All states confirmed.\n");
    }

    return countUnverified;
}

size_t verifyLosingStatesThread(const std::vector<Board>& losingStates, std::atomic<size_t>& counter, size_t logFreq) {
    size_t total = losingStates.size();
    size_t countUnverified = 0;

    while (true) {
        // Log progress
        size_t i = counter.fetch_add(1, std::memory_order_relaxed);
        if (i >= total) break;
        if ((i + 1) % logFreq == 0 || i + 1 == total) {
            printf("[Verify Losing] %zu / %zu\n", i, total);
        }

        const Board& board = losingStates[i];

        // Get all pusher moves
        std::vector<PusherMove> pusherMoves;
#ifdef USE_PRUNED_MOVES_FOR_VERIFICATION
        getAllPusherMovesPruned(board, pusherMoves);
#else
        pusherMoves = getAllPusherMoves(board);
#endif
        bool canGuaranteeLosing = std::all_of(
                pusherMoves.begin(), pusherMoves.end(),
                [&losingStates, i](const PusherMove& pusherMove) {
                    // Apply pusher move
                    Board board1 = losingStates[i];
                    applyPusherMove(board1, pusherMove);

                    // Get all remover moves
                    std::vector<int> removerMoves;
                    getAllRemoverMovesPruned(board1, removerMoves);

                    // If all moves lead to Pusher losing or another losing state, then this is a losing state
                    return std::any_of(
                            removerMoves.begin(), removerMoves.end(),
                            [&losingStates, board1](const int removerMove) {
                                Board board2 = board1;
                                applyRemoverMove(board2, removerMove);

                                // Check if state is losing or this board is a confirmed "losing state"
                                if (checkStatus(board2) == "LOSING") return true;
                                return boardIsLosing(board2, losingStates.begin(), losingStates.end());
                            });
                });

        if (!canGuaranteeLosing) {
            countUnverified++;
            printf("The following board is not confirmed as a losing state:\n%s\n", board.serialize().c_str());
        }
    }

    return countUnverified;
}

size_t verifyLosingStates(const std::vector<Board>& losingStates, size_t j, size_t logFreq) {
    // Sort the losing states by their number of tokens
    std::vector<Board> losingStates_ = losingStates;
    std::sort(losingStates_.begin(), losingStates_.end(), [](const Board& board1, const Board& board2) {
        return board1.num_tokens > board2.num_tokens;
    });

    // Define variables
    size_t total = losingStates_.size();
    std::atomic<size_t> counter = 0;
    std::vector<std::future<size_t>> futures;
    futures.reserve(j);

    // Create threads
    for (size_t index = 0; index < j; index++) {
        futures.push_back(std::async(
                std::launch::async,
                verifyLosingStatesThread,
                std::ref(losingStates_), std::ref(counter), logFreq));
    }

    // Join threads
    size_t countUnverified = 0;
    for (auto& fut : futures) {
        countUnverified += fut.get();
    }

    // Print results
    printf("Verified all losing states.\n");
    if (countUnverified) {
        printf("%zu (out of %zu) states not confirmed.\n", countUnverified, total);
    } else {
        printf("All states confirmed.\n");
    }

    return countUnverified;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <JSON config file>\n", argv[0]);
        exit(1);
    }

    // Load config
    nlohmann::json config;
    std::filesystem::path configFilePath = argv[1];
    std::ifstream fs(configFilePath);
    if (fs.is_open()) {
        fs >> config;
        fs.close();
    } else {
        fprintf(stderr, "Config file cannot be opened\n");
        exit(1);
    }

    // Parameters
    std::vector<std::pair<int, int>> k_and_n;
    load_k_and_n(k_and_n, config["common"]["k-and-n"]);
    int GOAL = config["common"]["goal"];  // Paintability = GOAL + 1

    // Initialize board
    int N, K;
    std::vector<std::vector<std::pair<int, int>>> startingBoard;
    createBoard(startingBoard, N, K, k_and_n);

    // Load the winning states
    auto [WINNING_FILE, LOSING_FILE] = getFileNames(N, K, GOAL);
    std::vector<Board> winningBoard, losingBoard;
    loadBoardsFromFile(WINNING_FILE, winningBoard);
    loadBoardsFromFile(LOSING_FILE, losingBoard);

    // Check if board is winning or losing
    Board board(N, K, GOAL, startingBoard);
    bool pusherWillWin = boardIsWinning(board, winningBoard.begin(), winningBoard.end());
    bool pusherWillLose = boardIsLosing(board, losingBoard.begin(), losingBoard.end());

    if (pusherWillWin && pusherWillLose) {
        printf("\nPrediction not available (starting state is in both winning and losing states).\n");
    } else if (!pusherWillWin && !pusherWillLose) {
        printf("\nPrediction not available (starting state is in neither winning nor losing states).\n");
        size_t unverifiedWinningStates = verifyWinningStates(
                winningBoard,
                config["verify"]["threads"],
                config["verify"]["log-frequency"]["winning"]);
        printf("\n");
        size_t unverifiedLosingStates = verifyLosingStates(
                losingBoard,
                config["verify"]["threads"],
                config["verify"]["log-frequency"]["losing"]);
        printf("\n");
        printf("Summary:\nPrediction not available.\n");
        printf("Winning states: %zu unconfirmed.\n", unverifiedWinningStates);
        printf("Losing states: %zu unconfirmed.\n", unverifiedLosingStates);
    } else {
        if (pusherWillWin) {
            size_t unverifiedWinningStates = verifyWinningStates(
                    winningBoard,
                    config["verify"]["threads"],
                    config["verify"]["log-frequency"]["winning"]);
            printf("\n");

            printf("Summary:\nPusher predicted to win");
            if (unverifiedWinningStates) {
                printf(" (%zu states unconfirmed).\n", unverifiedWinningStates);
            } else {
                printf(" (confirmed).\n");
            }
        } else {
            size_t unverifiedLosingStates = verifyLosingStates(
                    losingBoard,
                    config["verify"]["threads"],
                    config["verify"]["log-frequency"]["losing"]);
            printf("\n");

            printf("Summary:\nPusher predicted to lose");
            if (unverifiedLosingStates) {
                printf(" (%zu states unconfirmed).\n", unverifiedLosingStates);
            } else {
                printf(" (confirmed).\n");
            }
        }
    }

    return 0;
}
