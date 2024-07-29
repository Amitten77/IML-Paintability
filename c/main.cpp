//#define RECORD_PARTIAL_RESULT

#include <chrono>
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
    printf("<Initializing game state>\n");
    GameState initialGameState = initGameState(config);
    const Board& initialBoard = initialGameState.getBoard();
    size_t N = initialBoard.getN();
    size_t K = initialBoard.getK();
    int GOAL = initialGameState.getGoal();  // Paintability = GOAL + 1
    printf("N: %zu, K: %zu, GOAL: %d\n", N, K, GOAL);
    printf("Initial board:\n%s\n", initialBoard.toString().c_str());

    // Initialize archive
    printf("<Initializing archive>\n");
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
    printf("<Negamax start>\n");
    auto start = std::chrono::high_resolution_clock::now();
    size_t count;
    Player winner = minimax(initialGameState, archive, count);

    // End negamax algorithm
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::seconds duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    printf("<Negamax end>\n");
    printf("Total number of cases evaluated: %zu\n", count);
    printf("Winner: %s\n", toString(winner).c_str());
    printf("Execution time: %lld seconds\n", duration.count());

    // Save the winning and losing states to files
    printf("<Saving winning and losing states>\n");
    archive.prune();
    archive.saveWinning(winningFilename);
    archive.saveLosing(losingFilename);

    return 0;
}