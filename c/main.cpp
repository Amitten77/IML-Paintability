#define RUN_ONCE

#include <limits>
#include <chrono> // Include for high-resolution timing
#include <filesystem>
#include "include/Board.h"
#include "include/board_operation.h"
#include "include/helper.h"
#include "include/compare.h"
#include "include/graph.h"


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
int main() {
    // Start measuring time
    auto start = std::chrono::high_resolution_clock::now();

    // Parameters
    std::vector<std::pair<int, int>> k_and_n = { {1, 2}, {3, 5} };
    int GOAL = 7;  // Paintability = GOAL + 1
    std::string WINNING_FILE_LOAD;  // "winning/2024-05-07_07-17.txt";
    std::string LOSING_FILE_LOAD;  // "losing/2024-05-07_07-17.txt";

    // Initialize board
    int N, K;
    std::vector<std::vector<std::pair<int, int>>> startingBoard;
    createBoard(startingBoard, N, K, k_and_n);
    initMap(N, K);

    // Files
    auto [WINNING_FILE, LOSING_FILE] = getFileNames(N, K, GOAL);
    if (WINNING_FILE_LOAD.empty()) {
        WINNING_FILE_LOAD = WINNING_FILE;
    }
    if (LOSING_FILE_LOAD.empty()) {
        LOSING_FILE_LOAD = LOSING_FILE;
    }
    if (std::filesystem::exists(WINNING_FILE_LOAD)) {
        loadBoardsFromFile(WINNING_FILE_LOAD, WINNING);
    }
    if (std::filesystem::exists(LOSING_FILE_LOAD)) {
        loadBoardsFromFile(LOSING_FILE_LOAD, LOSING);
    }

    // Use negamax to find answer
    Board board(N, K, GOAL, startingBoard);
    printf("N: %d, K: %d\n", N, K);
    printf("Starting board:\n%s\n", board.serialize().c_str());

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