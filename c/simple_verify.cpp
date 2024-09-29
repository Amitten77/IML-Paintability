/**
 * @file simple_verify.cpp
 * @brief This file is used to verify the correctness of winning and losing states.
 *
 * Compared to `verify.cpp`, this file prioritizes simplicity over performance. To compile this file, see the
 * instructions in README.md.
 *
 * To run the compiled program, use the following command:
 * ```
 * ./simple_verify [N] [K] [GOAL]
 * ```
 *
 * Where `N` is the number of columns, `K` is the number of chips per column, and `GOAL` is the target row to reach. It
 * will then load the files `winning/N[N]_K[K]_goal[GOAL].txt` and `losing/N[N]_K[K]_goal[GOAL].txt`.
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
#include "init.h"

/**
 * @brief Verify if winning states are indeed winning.
 * @param archive Archive containing the exact list of winning states to verify.
 * @param goal The target row to reach.
 * @return The number of unverified states.
 */
size_t verifyWinningStates(const Archive& archive, int goal) {
    std::vector<Board> winningStates = archive.getWinningBoardsAsVector();
    size_t total = winningStates.size();
    size_t countUnverified = 0;

    for (size_t i = 0; i < total; i++) {
        // Fetch the game state to verify
        GameState state(winningStates[i], goal);

        // For any pusher move...
        std::vector<GameState> nextStates = state.step();
        bool isConfirmed = std::ranges::any_of(nextStates,
                [&archive](const GameState& nextState) {
                    // For all subsequent remover moves...
                    std::vector<GameState> nextNextStates = nextState.step();

                    // If lead to Pusher victory or a winning state, then original game state is confirmed
                    return std::ranges::all_of(nextNextStates,
                            [nextState, &archive](const GameState& nextNextState) {
                                return archive.predictWinner(nextNextState) == Player::PUSHER;
                            });
                });

        if (!isConfirmed) {
            countUnverified++;
        }

        // Log the current progress
        printf("\033[2K\033[G[Verify winning] %zu / %zu: %zu unverified", i + 1, total, countUnverified);
    }

    printf("\n");

    return countUnverified;
}

/**
 * @brief Verify if losing states are indeed losing.
 * @param archive Archive containing the exact list of losing states to verify.
 * @param goal The target row to reach.
 * @return The number of unverified states.
 */
size_t verifyLosingStates(const Archive& archive, int goal) {
    std::vector<Board> losingStates = archive.getLosingBoardsAsVector();
    size_t total = losingStates.size();
    size_t countUnverified = 0;

    for (size_t i = 0; i < total; i++) {
        // Fetch the game state to verify
        GameState state(losingStates[i], goal);

        // For all pusher moves...
        std::vector<GameState> nextStates = state.step();
        bool isConfirmed = std::ranges::all_of(nextStates,
                [&archive](const GameState& nextState) {
                    // For any subsequent remover move...
                    std::vector<GameState> nextNextStates = nextState.step();

                    // If lead to Remover victory or a losing state, then original game state is confirmed
                    return std::ranges::any_of(nextNextStates,
                            [&archive](const GameState& nextNextState) {
                                return archive.predictWinner(nextNextState) == Player::REMOVER;
                            });
                });

        if (!isConfirmed) {
            countUnverified++;
        }

        // Log the current progress
        printf("\033[2K\033[G[Verify losing] %zu / %zu: %zu unverified", i + 1, total, countUnverified);
    }

    printf("\n");

    return countUnverified;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [N] [K] [GOAL]\n", argv[0]);
        exit(1);
    }

    // Read command line arguments
    size_t N = std::stoul(argv[1]);
    size_t K = std::stoul(argv[2]);
    int GOAL = std::stoi(argv[3]);
    printf("N: %zu, K: %zu, GOAL: %d\n", N, K, GOAL);

    // Initialize starting game state
    printf("\n[Initializing game state]\n");
    GameState startingGameState(Board(N, K), GOAL);
    const Board& startingBoard = startingGameState.getBoard();
    printf("Initial board:\n%s", startingBoard.toString().c_str());

    // Load the winning and losing states
    printf("\n[Loading winning and losing states]\n");
    auto [winningFilename, losingFilename] = getFileNames(N, K, GOAL);

    // Create the archive (archives are libraries of winning & losing states that make it easier to compare game states)
    // Note we need two archives because the winning states and losing states are not verified, thus unreliable.
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
    verifyWinningStates(winningArchive, GOAL);
    verifyLosingStates(losingArchive, GOAL);

    return 0;
}
