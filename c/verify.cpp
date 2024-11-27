/**
 * @file verify.cpp
 * @brief This file is used to verify the correctness of winning and losing states.
 *
 * Same algorithm as `simple_verify.cpp`, but uses the configuration file to determine the starting game state. This
 * allows complex starting game states to be verified, such as those with multiple groups of K and N.
 */

#include <algorithm>
#include <fstream>
#include <vector>
#include "archive.h"
#include "board.h"
#include "game_state.h"
#include "helper.h"
#include "init.h"

/**
 * @brief Verify if winning states are indeed winning.
 * @param archive Archive containing the exact list of winning states to verify.
 * @param goal The target row to reach.
 * @param symmetric Whether the game is symmetric.
 * @param threads The number of threads to use.
 * @return The number of states failed to verify.
 */
size_t verifyWinningStates(const Archive& archive, int goal, bool symmetric, size_t threads) {
    std::vector<Board> winningStates = archive.getWinningBoardsAsVector();
    size_t total = winningStates.size();
    size_t numFailedToVerify = 0;

    // If there are no winning states, then there is nothing to verify
    if (total == 0) {
        printf("Verify winning: 0 / 0 (0 failed to verify)\n");
        return 0;
    }

    // Otherwise, verify the winning states one by one
    for (size_t i = 0; i < total; i++) {
        // Fetch the game state to verify
        GameState state(winningStates[i], goal, symmetric);

        // For any pusher move...
        std::vector<GameState> nextStates = state.step();
        bool isVerified = std::ranges::any_of(nextStates,
                [&archive, threads](const GameState& nextState) {
                    // For all subsequent remover moves...
                    std::vector<GameState> nextNextStates = nextState.step();

                    // If lead to Pusher victory or a winning state, then original game state is verified
                    return std::ranges::all_of(nextNextStates,
                            [nextState, &archive, threads](const GameState& nextNextState) {
                                return archive.predictWinner(nextNextState, threads) == Player::PUSHER;
                            });
                });

        if (!isVerified) {
            numFailedToVerify++;
        }

        // Log the current progress
        printf("\033[2K\033[GVerify winning: %zu / %zu (%zu failed to verify)", i + 1, total, numFailedToVerify);
    }

    printf("\n");

    return numFailedToVerify;
}

/**
 * @brief Verify if losing states are indeed losing.
 * @param archive Archive containing the exact list of losing states to verify.
 * @param goal The target row to reach.
 * @param symmetric Whether the game is symmetric.
 * @param threads The number of threads to use.
 * @return The number of states failed to verify.
 */
size_t verifyLosingStates(const Archive& archive, int goal, bool symmetric, size_t threads) {
    std::vector<Board> losingStates = archive.getLosingBoardsAsVector();
    size_t total = losingStates.size();
    size_t numFailedToVerify = 0;

    // If there are no losing states, then there is nothing to verify
    if (total == 0) {
        printf("Verify losing: 0 / 0 (0 failed to verify)\n");
        return 0;
    }

    // Otherwise, verify the losing states one by one
    for (size_t i = 0; i < total; i++) {
        // Fetch the game state to verify
        GameState state(losingStates[i], goal, symmetric);

        // For all pusher moves...
        std::vector<GameState> nextStates = state.step();
        bool isVerified = std::ranges::all_of(nextStates,
                [&archive, threads](const GameState& nextState) {
                    // For any subsequent remover move...
                    std::vector<GameState> nextNextStates = nextState.step();

                    // If lead to Remover victory or a losing state, then original game state is verified
                    return std::ranges::any_of(nextNextStates,
                            [&archive, threads](const GameState& nextNextState) {
                                return archive.predictWinner(nextNextState, threads) == Player::REMOVER;
                            });
                });

        if (!isVerified) {
            numFailedToVerify++;
        }

        // Log the current progress
        printf("\033[2K\033[GVerify losing: %zu / %zu (%zu failed to verify)", i + 1, total, numFailedToVerify);
    }

    printf("\n");

    return numFailedToVerify;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [JSON config file]\n", argv[0]);
        exit(1);
    }

    // Load config
    printf("[Loading config]\n");
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
    printf("\n[Initializing game state]\n");
    GameState startingGameState = initGameState(config);
    const Board& startingBoard = startingGameState.getBoard();
    size_t N = startingBoard.getN();
    size_t K = startingBoard.getK();
    int GOAL = startingGameState.getGoal();
    bool symmetric = startingGameState.isSymmetric();
    printf("N: %zu, K: %zu, GOAL: %d, SYM: %s\n", N, K, GOAL, symmetric ? "yes" : "no");
    printf("Starting board:\n%s", startingBoard.toString().c_str());

    // Load the winning and losing states
    printf("\n[Loading winning and losing states]\n");
    std::filesystem::path filename = getFilename(N, K, GOAL, symmetric);
    std::filesystem::path winningFilename = "winning" / filename;
    std::filesystem::path losingFilename = "losing" / filename;

    // Create the archive
    // Archives organize game states in a way that is easy to do batch comparisons.
    Archive winningArchive, losingArchive;
    winningArchive.loadWinning(winningFilename);
    losingArchive.loadLosing(losingFilename);

    // Check if starting game state is winning or losing
    printf("\n[Verification]\n");
    bool pusherWillWin = winningArchive.predictWinner(startingGameState) == Player::PUSHER;
    bool pusherWillLose = losingArchive.predictWinner(startingGameState) == Player::REMOVER;

    // Step 1: Determine if the starting state is in a winning or losing state
    std::string prediction;
    if (pusherWillWin && pusherWillLose) {
        prediction = "Starting state is both winning and losing state.";
        printf("Error: %s\n", prediction.c_str());
    } else if (pusherWillWin) {
        prediction = "Starting state is winning state.";
        printf("%s\n", prediction.c_str());
    } else if (pusherWillLose) {
        prediction = "Starting state is losing state.";
        printf("%s\n", prediction.c_str());
    } else {
        prediction = "Starting state is neither winning nor losing state.";
        printf("Error: %s\n", prediction.c_str());
    }

    // Step 2: Verify the winning and losing states
    verifyWinningStates(winningArchive, GOAL, symmetric, config["verify"]["threads"]);
    verifyLosingStates(losingArchive, GOAL, symmetric, config["verify"]["threads"]);

    return 0;
}
