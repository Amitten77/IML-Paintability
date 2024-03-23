#include <limits>
#include <chrono> // Include for high-resolution timing
#include "include/Board.h"
#include "include/helper.h"
#include "include/compare.h"
#include "include/graph.h"



/*
g++ -std=c++20 -O3 -flto -march=native -o main main.cpp src/Board.cpp src/helper.cpp src/graph.cpp src/compare.cpp
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
    int N = 6;
    int K = 3;
    int GOAL = 8;
    std::stringstream losing_ss;
    losing_ss << "losing/N" << N << "_K" << K << "_goal" << GOAL << "_board.txt";
    std::string LOSING_FILE = losing_ss.str();

    std::stringstream winning_ss;
    winning_ss << "winning/N" << N << "_K" << K << "_goal" << GOAL << "_board.txt";
    std::string WINNING_FILE = winning_ss.str();

    loadBoardsFromFile(LOSING_FILE, LOSING);
    loadBoardsFromFile(WINNING_FILE, WINNING);
    initMap(N, K);
    std::vector<std::vector<std::pair<int, int>>> curr = {
                {{3, 0}, {3, 0}, {3, 0}}, // Row 0
                {{3, 0}, {3, 0}, {3, 0}}, // Row 1
                {{3, 0}, {3, 0}, {3, 0}}, // Row 2
                {{3, 0}, {3, 0}, {3, 0}}, // Row 3
                {{3, 0}, {3, 0}, {3, 0}}, // Row 4
                {{3, 0}, {3, 0}, {3, 0}}  // Row 5
            };

    int val = 0;
    while (true) {
        val = val % 18;
        if (val == 0 && curr[int(val/6)][val % 3].first == 0) {
            break;
        } else {
            curr[int(val/3)][val % 3].first -= 1;
            val += 1;
        }
        Board myBoard(N, K, GOAL, curr);
        std::cout << "CURRENT BOARD" << std::endl;
        std::cout << myBoard << std::endl;
        int best = negaMax(myBoard, true, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0);
        std::cout << "BEST SCORE WITH NEGAMAX: " << best << std::endl;
    }
    


    saveBoardsToFile(LOSING, LOSING_FILE);
    saveBoardsToFile(WINNING, WINNING_FILE);
    // Stop measuring time and calculate the elapsed duration
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}