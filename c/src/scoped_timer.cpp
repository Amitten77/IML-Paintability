#include <cstdio>
#include "scoped_timer.h"

ScopedTimer::ScopedTimer() {
    this->startWall = std::chrono::steady_clock::now();
    this->startCpu = std::clock();
}

ScopedTimer::~ScopedTimer() {
    auto endWall = std::chrono::steady_clock::now();
    auto endCpu = std::clock();
    double durationWall = (double)std::chrono::duration_cast<std::chrono::seconds>(endWall - startWall).count();
    double durationCpu = static_cast<double>(endCpu - startCpu) / CLOCKS_PER_SEC;

    printf("Wall time: %.2f seconds\n", durationWall);
    printf("CPU time: %.2f seconds\n", durationCpu);
    printf("Speedup: %f\n", durationCpu / durationWall);
}
