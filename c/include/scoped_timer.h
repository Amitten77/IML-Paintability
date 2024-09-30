#ifndef SCOPED_TIMER_H
#define SCOPED_TIMER_H

#include <chrono>

class ScopedTimer {
public:
    ScopedTimer();
    ~ScopedTimer();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startWall;
    std::clock_t startCpu;
};

#endif //SCOPED_TIMER_H
