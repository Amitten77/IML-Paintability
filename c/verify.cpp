#include <algorithm>
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

#define NUM_THREADS 24
//#define USE_PRUNED_MOVES_FOR_VERIFICATION

// Verify the correctness of Pusher strategy
// 1. We provide the starting state and a list of "winning states" to the system.
// 2. The system will generate all possible moves of the Pusher and Remover and check if the Pusher can guarantee the
//    next game state is a winning state.
// 3. If not, then our results are incorrect (does not necessarily mean that the Pusher will lose though).
// 4. If yes, then let the Pusher make that move and continue to verify the resulting states (6 in total).
// 5. The check terminates when all paths lead to the Pusher winning (actually winning, not by comparing to
//    the list of "winning states").
void verifyPusherWin(const Board& board, const std::vector<Board>& winningStates) {
    std::queue<Board> boards;
    boards.push(board);

//    int pusherWinTarget = board.goal;
//    int removerWinTarget = board.num_tokens;
//    int pusherWinProgress = -1;
//    int removerWinProgress = -1;

    while (!boards.empty()) {
        Board curr = boards.front();
        boards.pop();
//         printf("%s\n", curr.serialize().c_str());

        // If already won, no need to continue
        if (checkWinner(curr) == Player::PUSHER) break;

        bool foundWinningMove = false;
        // Find all possible moves of the pusher
        std::vector<PusherMove> pusherMoves;
        getAllPusherMovesPruned(curr, pusherMoves);
        for (const PusherMove& pusherMove : pusherMoves) {
            // Apply pusher's move
            Board afterPusher = curr;
            applyPusherMove(afterPusher, pusherMove);

            foundWinningMove = true;
            std::vector<Board> nextStates;

            // Find all possible moves of the remover
            std::vector<int> removerMoves;
            getAllRemoverMovesPruned(curr, removerMoves);
            for (int removerMove : removerMoves) {
                Board afterRemover = afterPusher;
                applyRemoverMove(afterRemover, removerMove);

                // If pusher will win, remover should not take this move
                if (checkWinner(afterRemover) == Player::PUSHER) continue;

                // Add to list of potential next states
                nextStates.push_back(afterRemover);

                // If this board is not a "winning state", Pusher should not take this move.
                bool isWinning = boardIsWinning(afterRemover, winningStates.begin(), winningStates.end());
                if (!isWinning) {
                    foundWinningMove = false;
                    break;
                }
            }

            // If pusher's move guarantees reaching a winning state, then no need to search other moves.
            if (foundWinningMove) {
                // Insert all generated state to the back of the queue
                for (const Board& next : nextStates) {
                    boards.push(next);
                }
                break;
            }
        }

        // If all moves lead to non-winning state, then the pusher fails
        if (!foundWinningMove) {
            printf("Failed to find a winning move for:\n%s\n", curr.serialize().c_str());
            printf("Verification aborted.\n");
            // No winning move for `curr`
            return;
        }
    }

    printf("Verified Pusher strategy against all possible Remover strategies.\n");
    printf("Verification successful.\n");
}

// Not implemented
void verifyRemoverWin(Board board, int goal);

size_t verifyWinningStates(const std::vector<Board>& winningStates) {
    // Sort the winning states by their number of tokens
    std::vector<Board> winningStates_ = winningStates;
    std::sort(winningStates_.begin(), winningStates_.end(), [](const Board& board1, const Board& board2) {
        return board1.num_tokens < board2.num_tokens;
    });

    size_t total = winningStates_.size();
    size_t countUnverified = 0;
    for (size_t i = 0; i < total; i++) {
        // Log progress
        if ((i + 1) % 10 == 0 || i + 1 == total) {
            printf("[Verify Winning] %zu / %zu\n", i + 1, total);
        }

        const Board& board = winningStates_[i];

        // Get all pusher moves
        std::vector<PusherMove> pusherMoves;
#ifdef USE_PRUNED_MOVES_FOR_VERIFICATION
        getAllPusherMovesPruned(board, pusherMoves);
#else
        pusherMoves = getAllPusherMoves(board);
#endif
        bool canGuaranteeWinning = std::any_of(
                pusherMoves.begin(), pusherMoves.end(),
                [&winningStates_, i](const PusherMove& pusherMove) {
                    // Apply pusher move
                    Board board1 = winningStates_[i];
                    applyPusherMove(board1, pusherMove);

                    // Get all remover moves
                    std::vector<int> removerMoves;
                    getAllRemoverMovesPruned(board1, removerMoves);

                    // If all moves lead to Pusher victory or a confirmed winning state, then this is a winning state
                    return std::all_of(
                            removerMoves.begin(), removerMoves.end(),
                            [&winningStates_, i, board1](const int removerMove) {
                                Board board2 = board1;
                                applyRemoverMove(board2, removerMove);

                                // Check if state is winning or this board is a confirmed "winning state"
                                if (checkStatus(board2) == "WINNING") return true;
                                return boardIsWinning(board2, winningStates_.begin(), winningStates_.begin() + i);
                            });
                });

        if (!canGuaranteeWinning) {
            countUnverified++;
            printf("The following board is not confirmed as a winning state:\n%s\n", board.serialize().c_str());
        }
    }

    printf("Verified all winning states.\n");
    if (countUnverified) {
        printf("%zu (out of %zu) states not confirmed.\n", countUnverified, total);
    } else {
        printf("All states confirmed.\n");
    }

    return countUnverified;
}

size_t verifyLosingStatesThread(size_t index, size_t j, const std::vector<Board>& losingStates, std::mutex& mutex, size_t& numProcessed) {
    size_t total = losingStates.size();
    size_t countUnverified = 0;
    for (size_t i = index; i < total; i += j) {
        // Log progress
        {
            std::lock_guard<std::mutex> guard(mutex);
            numProcessed++;
            if (numProcessed % 50 == 0 || numProcessed == total) {
                printf("[Verify Losing] %zu / %zu\n", numProcessed, total);
            }
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

                                // Check if state is winning or this board is a confirmed "winning state"
                                if (checkStatus(board2) == "LOSING") return true;
                                return boardIsLosing(board2, losingStates.begin(), losingStates.end());
                            });
                });

        if (!canGuaranteeLosing) {
            countUnverified++;
            printf("The following board is not confirmed as a losing state:\n%s\n", board.serialize().c_str());
        }
    }

    printf("Thread %zu done (unconfirmed: %zu).\n", index, countUnverified);

    return countUnverified;
}

size_t verifyLosingStates(const std::vector<Board>& losingStates) {
    // Sort the losing states by their number of tokens
    std::vector<Board> losingStates_ = losingStates;
    std::sort(losingStates_.begin(), losingStates_.end(), [](const Board& board1, const Board& board2) {
        return board1.num_tokens > board2.num_tokens;
    });

    // Define variables
    size_t total = losingStates_.size();
    size_t numProcessed = 0;
    std::mutex mutex;
    std::vector<std::future<size_t>> futures;
    size_t j = NUM_THREADS;  // Number of threads
    futures.reserve(j);

    // Create threads
    for (size_t index = 0; index < j; index++) {
        futures.push_back(std::async(
                std::launch::async,
                verifyLosingStatesThread,
                index, j, std::ref(losingStates_), std::ref(mutex), std::ref(numProcessed)));
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
    load_k_and_n(k_and_n, config["k-and-n"]);
    int GOAL = config["goal"];  // Paintability = GOAL + 1

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
    bool pusherWillWin = std::find(winningBoard.begin(), winningBoard.end(), board) != winningBoard.end();
    bool pusherWillLose = std::find(losingBoard.begin(), losingBoard.end(), board) != losingBoard.end();

    if (pusherWillWin && pusherWillLose) {
        printf("\nPrediction not available (starting state is in both winning and losing states).\n");
    } else if (!pusherWillWin && !pusherWillLose) {
        printf("\nPrediction not available (starting state is in neither winning nor losing states).\n");
        size_t unverifiedWinningStates = verifyWinningStates(winningBoard);
        printf("\n");
        size_t unverifiedLosingStates = verifyLosingStates(losingBoard);
        printf("\n");
        printf("Summary:\nPrediction not available.\n");
        printf("Winning states: %zu unconfirmed.\n", unverifiedWinningStates);
        printf("Losing states: %zu unconfirmed.\n", unverifiedLosingStates);
    } else {
        if (pusherWillWin) {
            size_t unverifiedWinningStates = verifyWinningStates(winningBoard);
            printf("\n");

            printf("Summary:\nPusher predicted to win");
            if (unverifiedWinningStates) {
                printf(" (%zu states unconfirmed).\n", unverifiedWinningStates);
            } else {
                printf(" (confirmed).\n");
            }
        } else {
            size_t unverifiedLosingStates = verifyLosingStates(losingBoard);
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
