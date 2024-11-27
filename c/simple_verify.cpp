/**
 * @file simple_verify.cpp
 * @brief This file is used to verify the correctness of winning and losing states.
 *
 * Compared to `verify.cpp`, this file loads N, K, and GOAL from the command line arguments instead of the configuration
 * file. This makes the program easier to read.
 *
 * See README.md for instructions on building the program. To run the program, use the following command:
 * ```
 * ./simple_verify [N] [K] [GOAL]
 * ```
 *
 * Where `N` is the number of columns, `K` is the number of chips per column, and `GOAL` is the target row to reach. The
 * program automatically loads the files `winning/N[N]_K[K]_goal[GOAL].txt` and `losing/N[N]_K[K]_goal[GOAL].txt`.
 *
 * The first step is to decide if the initial state (i.e. the state where all chips are at row 0) is a winning or
 * losing. This can be done by comparing the initial state with the winning and losing states. Note that this step is
 * purely for providing information, and is independent of the correctness of the winning and losing states.
 *
 * Then, BOTH the winning states and losing states will be checked for correctness. We define a winning state as correct
 * if there exists a pusher move, for all subsequent remover moves, the resulting state satisfies one of the following
 * conditions:
 *   1. One of the chips is at row `goal`.
 *   2. The resulting state is greater than or equal to a state in the list of winning states.
 *
 * Similarly, we define a losing state as correct if for all pusher moves, there exists a remover move, such that the
 * resulting state satisfies one of the following conditions:
 *   1. All chips are removed.
 *   2. The resulting state is less than or equal to a state in the list of losing states.
 *
 * The above arguments show that the winning/losing states are closed under a certain pusher/remover strategy.
 *
 * Note that we only care about the correctness of either the winning or the losing state, depending on whether the
 * Pusher or the Remover has the winning strategy. If the initial state is a winning state, then the Pusher has the
 * winning strategy. In this case, we only care about the correctness of the winning states. Similarly, if the initial
 * state is a losing state, then the Remover has the winning strategy, so we only care about the correctness of the
 * losing states.
 */

#include <algorithm>
#include <vector>
#include "archive.h"
#include "board.h"
#include "game_state.h"
#include "helper.h"

/**
 * @brief Verify if winning states are indeed winning.
 * @param archive Archive containing the exact list of winning states to verify.
 * @param goal The target row to reach.
 * @param symmetric Whether the game is symmetric.
 * @return The number of states failed to verify.
 */
size_t verifyWinningStates(const Archive& archive, int goal, bool symmetric) {
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
                [&archive](const GameState& nextState) {
                    // For all subsequent remover moves...
                    std::vector<GameState> nextNextStates = nextState.step();

                    // If lead to Pusher victory or a winning state, then original game state is verified
                    return std::ranges::all_of(nextNextStates,
                            [nextState, &archive](const GameState& nextNextState) {
                                return archive.predictWinner(nextNextState) == Player::PUSHER;
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
 * @return The number of states failed to verify.
 */
size_t verifyLosingStates(const Archive& archive, int goal, bool symmetric) {
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
                [&archive](const GameState& nextState) {
                    // For any subsequent remover move...
                    std::vector<GameState> nextNextStates = nextState.step();

                    // If lead to Remover victory or a losing state, then original game state is verified
                    return std::ranges::any_of(nextNextStates,
                            [&archive](const GameState& nextNextState) {
                                return archive.predictWinner(nextNextState) == Player::REMOVER;
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
    // Check if the number of arguments is valid
    if (argc != 4 && argc != 5) {
        fprintf(stderr, "Usage: %s [N] [K] [GOAL] [-sym]\n", argv[0]);
        exit(1);
    }

    // Validate the optional [-sym] argument if present
    bool symmetric = false;
    if (argc == 5) {
        if (strcmp(argv[4], "-sym") == 0) {
            symmetric = true;
        } else {
            fprintf(stderr, "Invalid argument: %s\n", argv[4]);
            fprintf(stderr, "Usage: %s [N] [K] [GOAL] [-sym]\n", argv[0]);
            exit(1);
        }
    }

    // Read command line arguments
    size_t N = std::stoul(argv[1]);
    size_t K = std::stoul(argv[2]);
    int GOAL = std::stoi(argv[3]);

    // Initialize starting game state
    printf("\n[Initializing game state]\n");
    printf("N: %zu, K: %zu, GOAL: %d\n", N, K, GOAL);
    GameState startingGameState(Board(N, K), GOAL, symmetric);
    const Board& startingBoard = startingGameState.getBoard();
    printf("Initial board:\n%s", startingBoard.toString().c_str());

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
    verifyWinningStates(winningArchive, GOAL, symmetric);
    verifyLosingStates(losingArchive, GOAL, symmetric);

    return 0;
}
