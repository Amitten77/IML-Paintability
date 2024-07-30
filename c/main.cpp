//#define RECORD_PARTIAL_RESULT

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include "json.hpp"
#include "archive.h"
#include "board.h"
#include "compare.h"
#include "init.h"
#include "minimax.h"

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
        fprintf(stderr, "Usage: %s <JSON config file>\n", argv[0]);
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
    GameState initialGameState = initGameState(config);
    const Board& initialBoard = initialGameState.getBoard();
    size_t N = initialBoard.getN();
    size_t K = initialBoard.getK();
    int GOAL = initialGameState.getGoal();  // Paintability = GOAL + 1
    printf("N: %zu, K: %zu, GOAL: %d\n", N, K, GOAL);
    printf("Initial board:\n%s", initialBoard.toString().c_str());

    // Initialize archive
    printf("\n[Initializing archive]\n");
    Archive archive;
    auto [winningFilename, losingFilename] = getFileNames(N, K, GOAL);
    archive.loadWinning(winningFilename);
    archive.loadLosing(losingFilename);
    for (std::filesystem::path additionalWinningFilename : config["negamax"]["files-to-load-from"]["winning"]) {
        archive.loadWinning(additionalWinningFilename);
    }
    for (std::filesystem::path additionalLosingFilename : config["negamax"]["files-to-load-from"]["losing"]) {
        archive.loadLosing(additionalLosingFilename);
    }
    archive.prune();

    // Start negamax algorithm
    printf("\n[Minimax start]\n");
    auto startWall = std::chrono::high_resolution_clock::now();
    auto startCpu = std::clock();
    size_t count;
    Player winner = minimax(initialGameState, archive, count);

    // End negamax algorithm
    auto endWall = std::chrono::high_resolution_clock::now();
    auto endCpu = std::clock();
    double durationWall = (double)std::chrono::duration_cast<std::chrono::seconds>(endWall - startWall).count();
    double durationCpu = static_cast<double>(endCpu - startCpu) / CLOCKS_PER_SEC;

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
    printf("Wall time: %.2f seconds\n", durationWall);
    printf("CPU time: %.2f seconds\n", durationCpu);
    printf("Speedup: %f\n", durationCpu / durationWall);

    // Save the winning and losing states to files
    printf("\n[Saving winning and losing states]\n");
    archive.prune();
    archive.saveWinning(winningFilename);
    archive.saveLosing(losingFilename);

    return 0;
}
