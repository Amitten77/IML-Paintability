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
#include "archive.h"
#include "board.h"
#include "compare.h"
#include "helper.h"
#include "init.h"

/**
 * @brief Verify if winning boards are indeed winning.
 * @param winningStates A list of winning states to verify.
 * @param archive Archive containing the exact list of winning states to verify.
 * @param counter A counter to keep track of the progress and distributing work among threads.
 * @param logFreq Log frequency.
 * @return The number of unverified states.
 */
size_t verifyWinningStatesThread(
        const std::vector<GameState>& winningStates, const Archive& archive, std::atomic<size_t>& counter, size_t logFreq) {

    size_t total = winningStates.size();
    size_t countUnverified = 0;

    while (true) {
        // Log progress
        size_t i = counter.fetch_add(1, std::memory_order_relaxed);
        if (i >= total) break;
        if ((i + 1) % logFreq == 0 || i + 1 == total) {
            printf("[Verify Winning] %zu / %zu\n", i + 1, total);
        }

        const GameState& gameState = winningStates[i];

        // Double check that the Pusher will move next
        if (gameState.getCurrentPlayer() != Player::PUSHER) {
            printf(
                    "Warning: Skipping the following state due to being the Remover's turn:\n%s\n",
                    gameState.getBoard().toString().c_str());
            countUnverified++;
            continue;
        }

        // Take a step
#ifdef USE_PRUNED_MOVES_FOR_VERIFICATION
        std::vector<GameState> nextStates = gameState.stepPruned();
#else
        std::vector<GameState> nextStates = gameState.step();
#endif
        bool canGuaranteeWinning = std::any_of(
                nextStates.begin(), nextStates.end(),
                [&archive](const GameState& nextState) {
                    // Take another step
#ifdef USE_PRUNED_MOVES_FOR_VERIFICATION
                    std::vector<GameState> nextNextStates = nextState.stepPruned();
#else
                    std::vector<GameState> nextNextStates = nextState.step();
#endif

                    // If all moves lead to Pusher victory or a confirmed winning state, then this is a winning state
                    return std::all_of(
                            nextNextStates.begin(), nextNextStates.end(),
                            [nextState, &archive](const GameState& nextNextState) {
                                return archive.predictWinner(nextNextState) == Player::PUSHER;
                            });
                });

        if (!canGuaranteeWinning) {
            countUnverified++;
            printf("The following board is not confirmed as a winning state:\n%s\n", gameState.getBoard().toString().c_str());
        }
    }

    return countUnverified;
}

size_t verifyWinningStates(const std::vector<GameState>& winningStates, size_t j, size_t logFreq) {
    // Create archive
    Archive archive;
    for (const GameState& gameState : winningStates) {
        archive.addWinning(gameState.getBoard());
    }

    // Define variables
    size_t total = winningStates.size();
    std::atomic<size_t> counter = 0;
    std::vector<std::future<size_t>> futures;
    futures.reserve(j);

    // Create threads
    for (size_t index = 0; index < j; index++) {
        futures.push_back(std::async(
                std::launch::async,
                verifyWinningStatesThread,
                std::ref(winningStates), std::ref(archive), std::ref(counter), logFreq));
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

size_t verifyLosingStatesThread(
        const std::vector<GameState>& losingStates, const Archive& archive, std::atomic<size_t>& counter, size_t logFreq) {

    size_t total = losingStates.size();
    size_t countUnverified = 0;

    while (true) {
        // Log progress
        size_t i = counter.fetch_add(1, std::memory_order_relaxed);
        if (i >= total) break;
        if ((i + 1) % logFreq == 0 || i + 1 == total) {
            printf("[Verify Losing] %zu / %zu\n", i, total);
        }

        const GameState& gameState = losingStates[i];

        // Double check that the Remover will move next
        if (gameState.getCurrentPlayer() != Player::REMOVER) {
            printf(
                    "Warning: Skipping the following state due to being the Pusher's turn:\n%s\n",
                    gameState.getBoard().toString().c_str());
            countUnverified++;
            continue;
        }

        // Take a step
#ifdef USE_PRUNED_MOVES_FOR_VERIFICATION
        std::vector<GameState> nextStates = gameState.stepPruned();
#else
        std::vector<GameState> nextStates = gameState.step();
#endif
        bool canGuaranteeLosing = std::all_of(
                nextStates.begin(), nextStates.end(),
                [&archive](const GameState& nextState) {
                    // Take another step
#ifdef USE_PRUNED_MOVES_FOR_VERIFICATION
                    std::vector<GameState> nextNextStates = nextState.stepPruned();
#else
                    std::vector<GameState> nextNextStates = nextState.step();
#endif

                    // If all moves lead to Pusher losing or another losing state, then this is a losing state
                    return std::any_of(
                            nextNextStates.begin(), nextNextStates.end(),
                            [&archive](const GameState& nextNextState) {
                                return archive.predictWinner(nextNextState) == Player::REMOVER;
                            });
                });

        if (!canGuaranteeLosing) {
            countUnverified++;
            printf("The following board is not confirmed as a losing state:\n%s\n", gameState.getBoard().toString().c_str());
        }
    }

    return countUnverified;
}

size_t verifyLosingStates(const std::vector<GameState>& losingStates, size_t j, size_t logFreq) {
    // Create archive
    Archive archive;
    for (const GameState& gameState : losingStates) {
        archive.addLosing(gameState.getBoard());
    }

    // Define variables
    size_t total = losingStates.size();
    std::atomic<size_t> counter = 0;
    std::vector<std::future<size_t>> futures;
    futures.reserve(j);

    // Create threads
    for (size_t index = 0; index < j; index++) {
        futures.push_back(std::async(
                std::launch::async,
                verifyLosingStatesThread,
                std::ref(losingStates), std::ref(archive), std::ref(counter), logFreq));
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
    printf("<Loading config>\n");
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

    // Initialize game state
    printf("\n<Initializing game state>\n");
    GameState initialGameState = initGameState(config);
    const Board& initialBoard = initialGameState.getBoard();
    size_t N = initialBoard.getN();
    size_t K = initialBoard.getK();
    int GOAL = initialGameState.getGoal();  // Paintability = GOAL + 1
    printf("N: %zu, K: %zu, GOAL: %d\n", N, K, GOAL);
    printf("Initial board:\n%s\n", initialBoard.toString().c_str());

    // Load the winning and losing states
    printf("\n<Loading winning and losing states>\n");
    auto [winningFilename, losingFilename] = getFileNames(N, K, GOAL);
    Archive winningArchive, losingArchive;
    winningArchive.loadWinning(winningFilename);
    losingArchive.loadLosing(losingFilename);
    std::vector<GameState> winningBoards, losingBoards;
    for (const Board& board : winningArchive.getWinningBoardsAsVector()) {
        winningBoards.emplace_back(board, GOAL);
    }
    for (const Board& board : losingArchive.getLosingBoardsAsVector()) {
        losingBoards.emplace_back(board, GOAL);
    }

    // Check if board is winning or losing
    bool pusherWillWin = winningArchive.predictWinner(initialGameState) == Player::PUSHER;
    bool pusherWillLose = losingArchive.predictWinner(initialGameState) == Player::REMOVER;

    if (pusherWillWin && pusherWillLose) {
        printf("\nPrediction not available (starting state is in both winning and losing states).\n");
    } else if (!pusherWillWin && !pusherWillLose) {
        printf("\nPrediction not available (starting state is in neither winning nor losing states).\n");
        size_t unverifiedWinningStates = verifyWinningStates(
                winningBoards,
                config["verify"]["threads"],
                config["verify"]["log-frequency"]["winning"]);
        printf("\n");
        size_t unverifiedLosingStates = verifyLosingStates(
                losingBoards,
                config["verify"]["threads"],
                config["verify"]["log-frequency"]["losing"]);
        printf("\n");
        printf("Summary:\nPrediction not available.\n");
        printf("Winning states: %zu unconfirmed.\n", unverifiedWinningStates);
        printf("Losing states: %zu unconfirmed.\n", unverifiedLosingStates);
    } else {
        if (pusherWillWin) {
            size_t unverifiedWinningStates = verifyWinningStates(
                    winningBoards,
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
                    losingBoards,
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
