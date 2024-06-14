#define RUN_ONCE

#include <limits>
#include <chrono> // Include for high-resolution timing
#include <filesystem>
#include <fstream>
#include "include/json.hpp"
#include "include/Board.h"
#include "include/board_operation.h"
#include "include/compare.h"
#include "include/graph.h"
#include "include/helper.h"
#include "include/init.h"


/*
g++ -std=c++20 -O3 -flto -march=native -o main main.cpp src/Board.cpp src/helper.cpp src/graph.cpp src/compare.cpp
./main
*/
/**
 * Using CMake:
 * ```shell
 * cmake -B build
 * cmake --build build --config Release
 * ./build/test/iml_test
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
    std::vector<std::pair<int, int>> k_and_n;
    load_k_and_n(k_and_n, config["common"]["k-and-n"]);
    int GOAL = config["common"]["goal"];  // Paintability = GOAL + 1

    // Initialize board
    int N, K;
    std::vector<std::vector<std::pair<int, int>>> startingBoard;
    createBoard(startingBoard, N, K, k_and_n);
    initMap(N, K);

    // Files and boards
    auto [WINNING_FILE, LOSING_FILE] = getFileNames(N, K, GOAL);
    if (std::filesystem::exists(WINNING_FILE)) {
        loadBoardsFromFile(WINNING_FILE, WINNING);
    }
    if (std::filesystem::exists(LOSING_FILE)) {
        loadBoardsFromFile(LOSING_FILE, LOSING);
    }
    for (std::filesystem::path winningFile : config["negamax"]["files-to-load-from"]["winning"]) {
        if (std::filesystem::exists(winningFile)) {
            printf("Loading winning boards from %s...\n", winningFile.string().c_str());
            loadBoardsFromFile(winningFile, WINNING);
        }
    }
    for (std::filesystem::path losingFile : config["negamax"]["files-to-load-from"]["losing"]) {
        if (std::filesystem::exists(losingFile)) {
            printf("Loading losing boards from %s...\n", losingFile.string().c_str());
            loadBoardsFromFile(losingFile, WINNING);
        }
    }
    prune_winning();
    prune_losing();

    Board board(N, K, GOAL, startingBoard);
    printf("N: %d, K: %d\n", N, K);
    printf("Starting board:\n%s\n", board.serialize().c_str());

    // Start measuring time
    auto start = std::chrono::high_resolution_clock::now();

    int best = negaMax(board, true, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0);

    std::cout << "BEST SCORE WITH NEGAMAX: " << best << std::endl;
    prune_winning();
    prune_losing();
    saveBoardsToFile(WINNING, WINNING_FILE);
    saveBoardsToFile(LOSING, LOSING_FILE);
    printf("Winning states saved to file: %s\n", WINNING_FILE.c_str());
    printf("Losing states saved to file: %s\n", LOSING_FILE.c_str());

    // Stop measuring time and calculate the elapsed duration
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}