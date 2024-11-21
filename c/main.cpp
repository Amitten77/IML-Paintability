//#define RECORD_PARTIAL_RESULT

#include <chrono>
#include <filesystem>
#include <fstream>
#include "json.hpp"
#include "archive.h"
#include "board.h"
#include "game_state.h"
#include "helper.h"
#include "init.h"
#include "minimax.h"
#include "scoped_timer.h"

/**
 * Using CMake:
 * ```shell
 * cmake -B build
 * cmake --build build --config Release
 * cd build
 * ./main <JSON config file>
 * ```
 */
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
    const Board& initialBoard = startingGameState.getBoard();
    size_t N = initialBoard.getN();
    size_t K = initialBoard.getK();
    int GOAL = startingGameState.getGoal();  // Paintability = GOAL + 1
    printf("N: %zu, K: %zu, GOAL: %d\n", N, K, GOAL);
    printf("Initial board:\n%s", initialBoard.toString().c_str());

    // Initialize archive
    printf("\n[Initializing archive]\n");
    Archive archive;
    std::filesystem::path filename = getFilename(N, K, GOAL);
    std::filesystem::path winningFilename = "winning" / filename;
    std::filesystem::path losingFilename = "losing" / filename;
    archive.loadWinning(winningFilename);
    archive.loadLosing(losingFilename);
    for (std::filesystem::path additionalWinningFilename : config["minimax"]["files-to-load-from"]["winning"]) {
        archive.loadWinning(additionalWinningFilename);
    }
    for (std::filesystem::path additionalLosingFilename : config["minimax"]["files-to-load-from"]["losing"]) {
        archive.loadLosing(additionalLosingFilename);
    }
    archive.prune(1);

    {
        ScopedTimer timer;

        // Start minimax algorithm
        printf("\n[Minimax start]\n");
        size_t count = 0;
        Player winner = minimax(startingGameState, archive, config["minimax"]["hours-per-save"], config["minimax"]["threads"], count);

        // End minimax algorithm
        printf("\n[Minimax end]\n");
        printf("Total number of cases evaluated: %zu\n", count);
        switch (winner) {
        case Player::PUSHER:
            printf("\033[38;2;0;38;255mWinner: Pusher\033[0m\n");
            break;
        case Player::REMOVER:
            printf("\033[38;2;255;95;5mWinner: Remover\033[0m\n");
            break;
        case Player::NONE:
            printf("Winner not found\n");
            break;
        }
    }

    // Save the winning and losing states to files
    archive.prune();
    printf("\n[Saving winning and losing states]\n");
    archive.saveWinning(winningFilename);
    archive.saveLosing(losingFilename);

    return 0;
}
