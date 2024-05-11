#define RUN_ONCE

#include <limits>
#include <chrono> // Include for high-resolution timing
#include "include/Board.h"
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
    int N = 5;
    int K = 3;
    int GOAL = 6;
    std::string suffix_to_load_from;  // "2024-05-07_07-17";

    std::stringstream losing_ss, losing_ss_load;
    losing_ss << "losing/N" << N << "_K" << K << "_goal" << GOAL << "_board.txt";
    losing_ss_load << "losing/N" << N << "_K" << K << "_goal" << GOAL << "_" << suffix_to_load_from << ".txt";
    std::string LOSING_FILE = losing_ss.str();
    std::string LOSING_FILE_LOAD = losing_ss_load.str();

    std::stringstream winning_ss, winning_ss_load;
    winning_ss << "winning/N" << N << "_K" << K << "_goal" << GOAL << "_board.txt";
    winning_ss_load << "winning/N" << N << "_K" << K << "_goal" << GOAL << "_" << suffix_to_load_from << ".txt";
    std::string WINNING_FILE = winning_ss.str();
    std::string WINNING_FILE_LOAD = winning_ss_load.str();

    if (!suffix_to_load_from.empty()) {
        loadBoardsFromFile(LOSING_FILE_LOAD, LOSING);
        loadBoardsFromFile(WINNING_FILE_LOAD, WINNING);
    }
    initMap(N, K);
    std::vector<std::vector<std::pair<int, int>>> curr(N, {
        {0, 0}, {0, 0}, {0, 0}
    });

    Board myBoard(N, K, GOAL, curr);
    std::cout << "CURRENT BOARD" << std::endl;
    std::cout << myBoard << std::endl;
    int best = negaMax(myBoard, true, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0);
    std::cout << "BEST SCORE WITH NEGAMAX: " << best << std::endl;
    saveBoardsToFile(LOSING, LOSING_FILE);
    saveBoardsToFile(WINNING, WINNING_FILE);

    // Stop measuring time and calculate the elapsed duration
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}