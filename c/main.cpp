#include <iostream>
#include <limits>
#include <chrono> // Include for high-resolution timing
#include "Board.h"
#include "helper.h"



/*
g++ -std=c++17 -O3 -flto -march=native -o main main.cpp Board.cpp helper.cpp 
*/
/**
 * Using CMake:
 * ```shell
 * cmake -B build
 * cmake --build build --config Release
 * ./build/<EXECUTABLE_NAME>
 * ```
 */
int main() {
    // Start measuring time
    auto start = std::chrono::high_resolution_clock::now();

    Board myBoard(6, 3, 9, {
        {{4, 0}, {4, 0}, {4, 0}}, // Row 0
        {{4, 0}, {4, 0}, {4, 0}}, // Row 1
        {{4, 0}, {4, 0}, {4, 0}}, // Row 2
        {{4, 0}, {4, 0}, {4, 0}}, // Row 3
        {{-1, 0}, {-1, 0}, {-1, 0}}, // Row 4
        {{-1, 0}, {-1, 0}, {-1, 0}}  // Row 5
    });
    initMap(6, 3);
    int best = negaMax(myBoard, true, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0);
    std::cout << "BEST SCORE WITH NEGAMAX: " << best << std::endl;

    // Stop measuring time and calculate the elapsed duration
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}