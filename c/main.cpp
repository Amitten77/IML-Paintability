//#define RECORD_PARTIAL_RESULT

#include <chrono>
#include <filesystem>
#include <fstream>
#include <limits>
#include "json.hpp"
#include "archive.h"
#include "board.h"
#include "board_operation.h"
#include "compare.h"
#include "graph.h"
#include "helper.h"
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
    int GOAL = config["common"]["goal"];  // Paintability = GOAL + 1

    // Initialize game state
    printf("\n<Initializing game state>\n");
    Board initialBoard = createBoard(loadKAndN(config["common"]["k-and-n"]));
    GameState initialGameState(initialBoard, GOAL);
    size_t N = initialGameState.getBoard().getN();
    size_t K = initialGameState.getBoard().getK();
    printf("N: %zu, K: %zu\n", N, K);
    printf("Initial board:\n%s\n", initialBoard.toString().c_str());

    // Initialize archive
    printf("\n<Initializing archive>\n");
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
    printf("\n<Negamax start>\n");
    auto start = std::chrono::high_resolution_clock::now();
    size_t count;
    int best = negamax(initialGameState, count);

    // End negamax algorithm
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::seconds duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    printf("\n<Negamax end>\n");
    printf("Total number of cases evaluated: %zu\n", count);
    printf("Best score: %d\n", best);
    printf("Execution time: %lld seconds\n", duration.count());

    // Save the winning and losing states to files
    printf("\n<Saving winning and losing states>\n");
    archive.saveWinning(winningFilename);
    archive.saveLosing(losingFilename);

    return 0;
}