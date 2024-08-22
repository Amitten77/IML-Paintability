#include <limits>
#include <chrono> // Include for high-resolution timing
#include <csignal>
#include "include/Board.h"
#include "include/helper.h"
#include "include/compare.h"

#define RUN_ONCE

/*
g++ -std=c++20 -O3 -fno-omit-frame-pointer -o main main.cpp src/Board.cpp src/helper.cpp src/compare.cpp


g++ -std=c++20 -funroll-loops -O3 -o main main.cpp src/Board.cpp src/helper.cpp src/compare.cpp
./main

/*
Column 0: [(2, 1), (2, 0), (2, 0)]
Column 1: [(1, 0), (2, 0), (3, 1)]
Max Score: 2

Column 0: [(1, 0), (2, 0), (3, 1)]
Column 1: [(2, 1), (2, 0), (2, 0)]
Max Score: 2
*/



/**
 * Using CMake:
 * ```shell
 * cmake -B build
 * cmake --build build --config Release
 * ./build/test/iml_test
 * ```
 */

/*
Paintability of K_{2*6}: 6
Paintability of K_{2*5, 3*1}: 7
Paintability of K_{2*4, 3*2}: 7
Paintability of K_{2*3, 3*3}: 7
Paintability of K_{2*2, 3*4}: 8
Paintability of K_{2*1, 3*5}: 8
Paintability of K_{3*6}: 8

Paintability of K_{2*7}: 7
Paintability of K_{2*6, 3*1}: 7
Paintability of K_{2*5, 3*2}:  7
Paintability of K_{2*4, 3*3}: 8
Paintability of K_{2*3, 3*4}: 8
Paintability of K_{2*2, 3*5}: 9
Paintability of K_{2*1,3*6}: 9
Paintability of K_{3*7}:  
*/

std::string LOSING_FILE;
std::string WINNING_FILE;

void signalHandler(int signal) {
    std::cout << "Interrupt signal (" << signal << ") received.\n";
    // Save the current state of boards
    saveBoardsToFile(LOSING, LOSING_FILE);
    saveBoardsToFile(WINNING, WINNING_FILE);
    std::cout << "Boards saved due to SIGINT.\n";
    // Exit gracefully
    exit(signal);
}


int main() {
    // Start measuring time

    std::signal(SIGINT, signalHandler);
    int N = 6;
    int K = 3;
    int GOAL = 8;
    initMap(N, K);

    std::stringstream losing_ss;
    losing_ss << "losing/N" << N << "_K" << K << "_goal" << GOAL << "_board.txt";
    LOSING_FILE = losing_ss.str();

    std::stringstream winning_ss;
    winning_ss << "winning/N" << N << "_K" << K << "_goal" << GOAL << "_board.txt";
    WINNING_FILE = winning_ss.str();

    loadBoardsFromFile(LOSING_FILE, LOSING, N, K, GOAL);
    loadBoardsFromFile(WINNING_FILE, WINNING, N, K, GOAL);



    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<std::pair<int, int>>> curr = {
                {{-1, 0}, {0, 0}, {0, 0}}, // Row 0
                {{-1, 0}, {0, 0}, {0, 0}}, // Row 1
                {{-1, 0}, {0, 0}, {0, 0}}, // Row 2
                {{0, 0}, {0, 0}, {0, 0}}, // Row 3
                {{0, 0}, {0, 0}, {0, 0}}, // Row 4
                {{0, 0}, {0, 0}, {0, 0}},
                {{0, 0}, {0, 0}, {0, 0}}, 
                {{0, 0}, {0, 0}, {0, 0}}
            };
    Board myBoard(N, K, GOAL, curr);

    int best = negaMax(myBoard, true, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0);
    std::cout << best << std::endl;

    // Stop measuring time and calculate the elapsed duration
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    std::cout << "Execution time: " << duration.count() << " seconds" << '\n';

    saveBoardsToFile(LOSING, LOSING_FILE);
    saveBoardsToFile(WINNING, WINNING_FILE);

    return 0;
}